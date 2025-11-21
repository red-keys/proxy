#pragma once      
      
#include "envoy/http/filter.h"      
#include "source/extensions/filters/http/common/pass_through_filter.h"    
#include "source/common/http/utility.h"    
#include "absl/strings/string_view.h"    
#include <fstream>  
#include <iterator>  
  
namespace Envoy {      
namespace Http {      
namespace SimpleFilter {      
      
class SimpleFilter : public PassThroughFilter {    
public:    
  SimpleFilter() = default;    
      
  FilterHeadersStatus decodeHeaders(RequestHeaderMap& headers, bool end_stream) override {    
    session_id_ = Http::Utility::parseCookieValue(headers, "sessionid");    
      
    // 保存 headers 引用以便后续使用  
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
        
    // 解析请求体    
    const auto* buffer = decoder_callbacks_->decodingBuffer();    
    if (buffer) {    
      std::string body = buffer->toString();    
          
      size_t start_pos = body.find("<SERVICE_CODE attr=\"s,30\">");    
      size_t end_pos = body.find("</SERVICE_CODE>");    
          
      if (start_pos != std::string::npos && end_pos != std::string::npos) {    
        start_pos += 26;    
        b_value_ = body.substr(start_pos, end_pos - start_pos);    
            
        // 根据解析结果设置路由选择头  
        if (request_headers_) {  
          if (b_value_ == "01001000001") {    
            request_headers_->addCopy(    
              LowerCaseString("x-mock-response"), "success");    
          } else {    
            request_headers_->addCopy(    
              LowerCaseString("x-mock-response"), "error");    
          }    
              
          // 清除路由缓存,强制重新匹配路由    
          decoder_callbacks_->clearRouteCache();    
        }  
      }    
    }    
        
    return FilterDataStatus::Continue;    
  }  
  
private:    
  std::string session_id_;    
  std::string b_value_;  
  RequestHeaderMap* request_headers_{nullptr};  // 保存请求头引用  
};   
} // namespace SimpleFilter      
} // namespace Http      
} // namespace Envoy