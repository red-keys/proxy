#include "src/envoy/http/simple_filter/simple_filter.h"  
  
namespace Envoy {  
namespace Http {  
namespace SimpleFilter {  
  
  FilterHeadersStatus SimpleFilter::decodeHeaders(RequestHeaderMap& headers, bool end_stream)  {  
    session_id_ = Http::Utility::parseCookieValue(headers, "sessionid");  
    request_headers_ = &headers;  
    if (!end_stream) {  
      return FilterHeadersStatus::StopIteration;  
    }  
    return FilterHeadersStatus::Continue;  
  }  

  FilterDataStatus SimpleFilter::decodeData(Buffer::Instance&, bool end_stream) {  
    if (!end_stream) {  
        return FilterDataStatus::StopIterationAndBuffer;  
    }  

    const auto* buffer = decoder_callbacks_->decodingBuffer();  
    if (buffer && buffer->length() >= 4) {   
      std::string xml_data;  
      xml_data.resize(buffer->length());  
      buffer->copyOut(0, buffer->length(), &xml_data[0]);  
          
      // 处理 XML 数据  
      size_t start_pos = xml_data.find(config_->xmlTagStart());  
      size_t end_pos = xml_data.find(config_->xmlTagEnd());  
      if (start_pos != std::string::npos && end_pos != std::string::npos) {  
          start_pos += config_->xmlTagStart().length();  
          b_value_ = xml_data.substr(start_pos, end_pos - start_pos);  
          if (request_headers_) {  
              if (config_->successMatchValue().empty() || b_value_ == config_->successMatchValue()) {  
                  request_headers_->addCopy(  
                      LowerCaseString("x-mock-response"), config_->successRouteMarker());  
                  decoder_callbacks_->clearRouteCache();  
                  return FilterDataStatus::Continue;  
              }  
          }  
      }       
    }  
    
    decoder_callbacks_->clearRouteCache();  
    return FilterDataStatus::Continue;  
  }
 
  Http::FilterHeadersStatus SimpleFilter::encodeHeaders(ResponseHeaderMap& headers, bool )  {  
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
 
    return Http::FilterHeadersStatus::Continue;  
  } 
} // namespace SimpleFilter  
} // namespace Http  
} // namespace Envoy