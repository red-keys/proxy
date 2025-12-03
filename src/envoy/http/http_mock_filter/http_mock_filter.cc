#include "src/envoy/http/http_mock_filter/http_mock_filter.h"  
  
namespace Envoy {  
namespace Http {  
namespace HttpMockFilter {  
  
  FilterHeadersStatus HttpMockFilter::decodeHeaders(RequestHeaderMap& headers, bool end_stream)  {   
    request_headers_ = &headers;  
    if (!end_stream) {  
      return FilterHeadersStatus::StopIteration;  
    }  
    return FilterHeadersStatus::Continue;  
  }  

  FilterDataStatus HttpMockFilter::decodeData(Buffer::Instance&, bool end_stream) {  
    if (!end_stream) {  
        return FilterDataStatus::StopIterationAndBuffer;  
    }  

    const auto* buffer = decoder_callbacks_->decodingBuffer();  
    if (buffer && buffer->length() >= 4) {   
      std::string xml_data;  
      xml_data.resize(buffer->length());  
      // 读取HTTP请求体内容到 xml_data 字符串中
      buffer->copyOut(0, buffer->length(), &xml_data[0]);  
      
      size_t first_pos = xml_data.find(config_->firstCandidateStr());  
      size_t second_pos = xml_data.find(config_->secondCandidateStr());  
      if (first_pos != std::string::npos && second_pos != std::string::npos) {  
        if (request_headers_) {  
          //做一个标记，表示走成功路由 路由规则匹配到成功标记的后续动作在配置文件定义
          request_headers_->addCopy(  
              LowerCaseString("x-mock-response"), config_->successRouteMarker());  
          decoder_callbacks_->clearRouteCache();  
          return FilterDataStatus::Continue;    
        }  
      }       
    }  
    
    decoder_callbacks_->clearRouteCache();  
    return FilterDataStatus::Continue;  
  }
 
  Http::FilterHeadersStatus HttpMockFilter::encodeHeaders(ResponseHeaderMap& headers, bool )  {  
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
          // 添加参数到响应HTTP头中
          headers.addViaMove(std::move(key_string), std::move(value_string));
        }  
      }  
    }  
 
    return Http::FilterHeadersStatus::Continue;  
  } 
} // namespace HttpMockFilter  
} // namespace Http  
} // namespace Envoy