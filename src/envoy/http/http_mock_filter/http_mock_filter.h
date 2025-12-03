#pragma once              
        
#include "envoy/http/filter.h"        
#include "source/extensions/filters/http/common/pass_through_filter.h"      
#include "source/common/http/utility.h"      
#include "absl/strings/string_view.h"      
#include "src/envoy/http/http_mock_filter/config/config.pb.h" 
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
namespace HttpMockFilter {      
  
// 过滤器配置类  
class FilterConfig {  
public:  
  FilterConfig(const http_mock_filter::HttpMockFilterConfig& proto_config)  
      : first_candidate_str_(proto_config.first_candidate_str()),  
        second_candidate_str_(proto_config.second_candidate_str()),  
        success_route_marker_(proto_config.success_route_marker()),  
        error_route_marker_(proto_config.error_route_marker()),
        response_header_(proto_config.response_header()),
        response_content_file_(proto_config.response_content_file()) {}  
  
  const std::string& firstCandidateStr() const { return first_candidate_str_; }  
  const std::string& secondCandidateStr() const { return second_candidate_str_; }   
  const std::string& successRouteMarker() const { return success_route_marker_; }  
  const std::string& errorRouteMarker() const { return error_route_marker_; }  
  const std::string& responseHeader() const { return response_header_; }
  const std::string& responseContentFile() const { return response_content_file_; }

private:  
  const std::string first_candidate_str_;  
  const std::string second_candidate_str_;  
  const std::string success_route_marker_;  
  const std::string error_route_marker_;  
  const std::string response_header_;
  const std::string response_content_file_;
};  
  
using FilterConfigSharedPtr = std::shared_ptr<FilterConfig>;  
      
class HttpMockFilter : public PassThroughFilter {   
public:  
  HttpMockFilter(FilterConfigSharedPtr config) : config_(config) {}  
  
  FilterHeadersStatus decodeHeaders(RequestHeaderMap& headers, bool end_stream) override ;
  
  FilterDataStatus decodeData(Buffer::Instance&, bool end_stream) override ; 
  
  Http::FilterHeadersStatus encodeHeaders(ResponseHeaderMap& headers, bool ) override ; 

private:  
  FilterConfigSharedPtr config_;  
  RequestHeaderMap* request_headers_{nullptr}; 
};
} // namespace HttpMockFilter      
} // namespace Http      
} // namespace Envoy