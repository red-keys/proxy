#pragma once              
        
#include "envoy/http/filter.h"        
#include "source/extensions/filters/http/common/pass_through_filter.h"      
#include "source/common/http/utility.h"      
#include "absl/strings/string_view.h"      
#include "src/envoy/http/simple_filter/config/config.pb.h"  // 修改这一行  
#include <fstream>    
#include <iterator>
#include <map>
#include <sstream>
#include "source/common/buffer/buffer_impl.h"

namespace Envoy {      
namespace Http {      
namespace SimpleFilter {      
  
// 过滤器配置类  
class FilterConfig {  
public:  
  FilterConfig(const simple_filter::SimpleFilterConfig& proto_config)  
      : xml_tag_start_(proto_config.xml_tag_start()),  
        xml_tag_end_(proto_config.xml_tag_end()),  
        success_match_value_(proto_config.success_match_value()),  
        success_route_marker_(proto_config.success_route_marker()),  
        error_route_marker_(proto_config.error_route_marker()),
        response_header_(proto_config.response_header()) {}  
  
  const std::string& xmlTagStart() const { return xml_tag_start_; }  
  const std::string& xmlTagEnd() const { return xml_tag_end_; }  
  const std::string& successMatchValue() const { return success_match_value_; }  
  const std::string& successRouteMarker() const { return success_route_marker_; }  
  const std::string& errorRouteMarker() const { return error_route_marker_; }  
  const std::string& responseHeader() const { return response_header_; }

private:  
  const std::string xml_tag_start_;  
  const std::string xml_tag_end_;  
  const std::string success_match_value_;  
  const std::string success_route_marker_;  
  const std::string error_route_marker_;  
  const std::string response_header_;
};  
  
using FilterConfigSharedPtr = std::shared_ptr<FilterConfig>;  
      
class SimpleFilter : public PassThroughFilter {   
public:  
  SimpleFilter(FilterConfigSharedPtr config) : config_(config) {}  
  
  FilterHeadersStatus decodeHeaders(RequestHeaderMap& headers, bool end_stream) override {  
    session_id_ = Http::Utility::parseCookieValue(headers, "sessionid");  
    request_headers_ = &headers;  
    if (!end_stream) {  
      return FilterHeadersStatus::StopIteration;  
    }  
    return FilterHeadersStatus::Continue;  
  }  
  
  FilterDataStatus decodeData(Buffer::Instance&, bool end_stream) override {  
    if (!end_stream) {  
      return FilterDataStatus::StopIterationAndBuffer;  
    }  
    const auto* buffer = decoder_callbacks_->decodingBuffer();  
    if (buffer) {  
      std::string body = buffer->toString();  
      // 使用配置中的标签名称  
      size_t start_pos = body.find(config_->xmlTagStart());  
      size_t end_pos = body.find(config_->xmlTagEnd());  
      if (start_pos != std::string::npos && end_pos != std::string::npos) {  
        start_pos += config_->xmlTagStart().length();  
        b_value_ = body.substr(start_pos, end_pos - start_pos);  
        if (request_headers_) {  
          // 使用配置中的匹配值和路由标记  
          if (config_->successMatchValue().empty() || b_value_ == config_->successMatchValue()) {  
            request_headers_->addCopy(  
                LowerCaseString("x-mock-response"), config_->successRouteMarker());  
          } else {  
            request_headers_->addCopy(  
                LowerCaseString("x-mock-response"), config_->errorRouteMarker());  
          }  
          decoder_callbacks_->clearRouteCache();  
        }  
      }  
    }  
    return FilterDataStatus::Continue;  
  }  
  
  Http::FilterHeadersStatus encodeHeaders(ResponseHeaderMap& headers, bool ) override {  
    // 解析 response_header_ 字符串，格式为 "userName:John;userId:123;"  
    std::string header_str = config_->responseHeader();  
      
    // 按 ';' 分割字符串  
    std::stringstream ss(header_str);  
    std::string pair;  
      
    while (std::getline(ss, pair, ';')) {  
      if (pair.empty()) continue;  
        
      // 按 ':' 分割键值对  
      size_t colon_pos = pair.find(':');  
      if (colon_pos != std::string::npos) {  
        std::string key = pair.substr(0, colon_pos);  
        std::string value = pair.substr(colon_pos + 1);  

        if (!key.empty() && !value.empty()) { 
          HeaderString key_string;
          key_string.setCopy(key);
          HeaderString value_string;
          value_string.setCopy(value);
          headers.addViaMove(std::move(key_string), std::move(value_string));
        }  
      }  
    }  
      
    // 读取文件内容  
    std::ifstream file("./http_response.txt", std::ios::binary);  
    std::string content((std::istreambuf_iterator<char>(file)),   
                        std::istreambuf_iterator<char>());  
      
    // 创建带长度前缀的二进制数据  
    uint32_t length = htonl(content.length());  
    uint32_t total_length = sizeof(length) + content.length();  
      
    // 设置正确的content-length（包含4字节长度头）  
    headers.setContentLength(total_length);  
      
    Buffer::OwnedImpl body_data;  
    body_data.add(&length, sizeof(length));  
    body_data.add(content);  
      
    // 注入响应体  
    encoder_callbacks_->addEncodedData(body_data, true);     
    return Http::FilterHeadersStatus::Continue;  
  } 
  
private:  
  FilterConfigSharedPtr config_;  
  std::string session_id_;  
  std::string b_value_;  
  RequestHeaderMap* request_headers_{nullptr}; 
};
} // namespace SimpleFilter      
} // namespace Http      
} // namespace Envoy