#include "src/envoy/http/http_mock_filter/config.h"    
#include "src/envoy/http/http_mock_filter/http_mock_filter.h"    
#include "src/envoy/http/http_mock_filter/config/config.pb.h"    
    
namespace Envoy {    
namespace Http {    
namespace HttpMockFilter {    
  
Http::FilterFactoryCb HttpMockFilterFactory::createFilterFactoryFromProto(    
    const Protobuf::Message& proto_config, const std::string&,    
    Server::Configuration::FactoryContext&) {    
    
  // 将 proto_config 转换为具体的配置类型  
  const auto& config = dynamic_cast<const http_mock_filter::HttpMockFilterConfig&>(proto_config);  
    
  // 创建 FilterConfig 对象  
  auto filter_config = std::make_shared<FilterConfig>(config);  
    
  return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) {    
    callbacks.addStreamFilter(std::make_shared<HttpMockFilter>(filter_config));    
  };    
}  
  
ProtobufTypes::MessagePtr HttpMockFilterFactory::createEmptyConfigProto() {    
  return std::make_unique<http_mock_filter::HttpMockFilterConfig>();  
}    
    
std::string HttpMockFilterFactory::name() const {    
  return "envoy.filters.http.http_mock_filter";    
}    
    
static Registry::RegisterFactory<HttpMockFilterFactory,    
                                 Server::Configuration::NamedHttpFilterConfigFactory>    
    registered_;    
    
} // namespace HttpMockFilter    
} // namespace Http    
} // namespace Envoy