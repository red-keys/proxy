#pragma once    
    
#include "envoy/http/filter.h"    
#include "source/extensions/filters/http/common/pass_through_filter.h"  
#include "source/common/http/utility.h"  
#include "absl/strings/string_view.h"  
    
namespace Envoy {    
namespace Http {    
namespace SimpleFilter {    
    
class SimpleFilter : public PassThroughFilter {    
public:    
  SimpleFilter() = default;    
      
  // 在请求头中解析 Cookie  
  FilterHeadersStatus decodeHeaders(RequestHeaderMap& headers, bool end_stream) override {    
    session_id_ = Http::Utility::parseCookieValue(headers, "sessionid");  
      
    // 如果有请求体,需要缓冲它  
    if (!end_stream) {  
      return FilterHeadersStatus::StopIteration;  
    }  
      
    return FilterHeadersStatus::Continue;    
  }    
    
  // 处理请求体数据  
  FilterDataStatus decodeData(Buffer::Instance& , bool end_stream) override {  
    // 将数据添加到缓冲区  
    if (!end_stream) {  
      // 继续缓冲直到收到所有数据  
      return FilterDataStatus::StopIterationAndBuffer;  
    }  
      
    // 获取完整的请求体  
    const auto* buffer = decoder_callbacks_->decodingBuffer();  
    if (buffer) {  
      std::string body = buffer->toString();  
        
      // 简单的 XML 解析 - 查找 <b>...</b>  
      size_t start_pos = body.find("<b>");  
      size_t end_pos = body.find("</b>");  
        
      if (start_pos != std::string::npos && end_pos != std::string::npos) {  
        start_pos += 3; // 跳过 "<b>"  
        b_value_ = body.substr(start_pos, end_pos - start_pos);  
      }  
    }  
      
    return FilterDataStatus::Continue;  
  }  
      
  // 在响应头中根据条件添加标头  
  FilterHeadersStatus encodeHeaders(ResponseHeaderMap& headers, bool) override {    
    // 检查 sessionid  
    if (session_id_ == "123") {  
      headers.addCopy(LowerCaseString("x-simple-filter"), "hello");  
    }  
      
    // 检查 b 标签的值  
    if (b_value_ == "188") {  
      headers.addCopy(LowerCaseString("x-simple1-filter"), "hello world");  
    }  
      
    return FilterHeadersStatus::Continue;    
  }  
  
private:  
  std::string session_id_;  
  std::string b_value_;  
};  
} // namespace SimpleFilter    
} // namespace Http    
} // namespace Envoy