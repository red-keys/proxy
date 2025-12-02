#pragma once              
        
#include "envoy/http/filter.h"        
#include "source/extensions/filters/http/common/pass_through_filter.h"      
#include "source/common/http/utility.h"      
#include "absl/strings/string_view.h"      
#include "src/envoy/http/simple_filter/config/config.pb.h" 
#include <fstream>    
#include <iterator>
#include <map>
#include <sstream>
#include "source/common/buffer/buffer_impl.h"
#include "source/common/common/base64.h"
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/err.h>

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
        response_header_(proto_config.response_header()),
        response_content_file_(proto_config.response_content_file()) {}  
  
  const std::string& xmlTagStart() const { return xml_tag_start_; }  
  const std::string& xmlTagEnd() const { return xml_tag_end_; }  
  const std::string& successMatchValue() const { return success_match_value_; }  
  const std::string& successRouteMarker() const { return success_route_marker_; }  
  const std::string& errorRouteMarker() const { return error_route_marker_; }  
  const std::string& responseHeader() const { return response_header_; }
  const std::string& responseContentFile() const { return response_content_file_; }

private:  
  const std::string xml_tag_start_;  
  const std::string xml_tag_end_;  
  const std::string success_match_value_;  
  const std::string success_route_marker_;  
  const std::string error_route_marker_;  
  const std::string response_header_;
  const std::string response_content_file_;
};  
  
using FilterConfigSharedPtr = std::shared_ptr<FilterConfig>;  
      
class SimpleFilter : public PassThroughFilter {   
public:  
  SimpleFilter(FilterConfigSharedPtr config) : config_(config) {}  
  
  FilterHeadersStatus decodeHeaders(RequestHeaderMap& headers, bool end_stream) override ;
  
  FilterDataStatus decodeData(Buffer::Instance&, bool end_stream) override ; 
  
  Http::FilterHeadersStatus encodeHeaders(ResponseHeaderMap& headers, bool ) override ; 

private:  
  FilterConfigSharedPtr config_;  
  std::string session_id_;  
  std::string b_value_;  
  RequestHeaderMap* request_headers_{nullptr}; 
};
} // namespace SimpleFilter      
} // namespace Http      
} // namespace Envoy