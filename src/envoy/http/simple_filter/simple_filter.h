#pragma once              
        
#include "envoy/http/filter.h"        
#include "source/extensions/filters/http/common/pass_through_filter.h"      
#include "source/common/http/utility.h"      
#include "absl/strings/string_view.h"      
#include "src/envoy/http/simple_filter/config/config.pb.h"  // 修改这一行  
#include <fstream>    
#include <iterator>
  
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
        error_route_marker_(proto_config.error_route_marker()) {}  
  
  const std::string& xmlTagStart() const { return xml_tag_start_; }  
  const std::string& xmlTagEnd() const { return xml_tag_end_; }  
  const std::string& successMatchValue() const { return success_match_value_; }  
  const std::string& successRouteMarker() const { return success_route_marker_; }  
  const std::string& errorRouteMarker() const { return error_route_marker_; }  
  
private:  
  const std::string xml_tag_start_;  
  const std::string xml_tag_end_;  
  const std::string success_match_value_;  
  const std::string success_route_marker_;  
  const std::string error_route_marker_;  
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
          if (b_value_ == config_->successMatchValue()) {    
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
  
private:    
  FilterConfigSharedPtr config_;  
  std::string session_id_;    
  std::string b_value_;  
  RequestHeaderMap* request_headers_{nullptr};  
};   
} // namespace SimpleFilter      
} // namespace Http      
} // namespace Envoy