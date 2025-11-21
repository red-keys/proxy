#include "src/envoy/http/simple_filter/config.h"    
#include "src/envoy/http/simple_filter/simple_filter.h"    
#include "src/envoy/http/simple_filter/config/config.pb.h"    
    
namespace Envoy {    
namespace Http {    
namespace SimpleFilter {    
  
Http::FilterFactoryCb SimpleFilterFactory::createFilterFactoryFromProto(    
    const Protobuf::Message& proto_config, const std::string&,    
    Server::Configuration::FactoryContext&) {    
    
  // 将 proto_config 转换为具体的配置类型  
  const auto& config = dynamic_cast<const simple_filter::SimpleFilterConfig&>(proto_config);  
    
  // 创建 FilterConfig 对象  
  auto filter_config = std::make_shared<FilterConfig>(config);  
    
  return [filter_config](Http::FilterChainFactoryCallbacks& callbacks) {    
    callbacks.addStreamFilter(std::make_shared<SimpleFilter>(filter_config));    
  };    
}  
  
ProtobufTypes::MessagePtr SimpleFilterFactory::createEmptyConfigProto() {    
  return std::make_unique<simple_filter::SimpleFilterConfig>();  
}    
    
std::string SimpleFilterFactory::name() const {    
  return "envoy.filters.http.simple_filter";    
}    
    
static Registry::RegisterFactory<SimpleFilterFactory,    
                                 Server::Configuration::NamedHttpFilterConfigFactory>    
    registered_;    
    
} // namespace SimpleFilter    
} // namespace Http    
} // namespace Envoy