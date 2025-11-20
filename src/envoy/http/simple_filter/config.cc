#include "src/envoy/http/simple_filter/config.h"  
#include "src/envoy/http/simple_filter/simple_filter.h"  
#include "src/envoy/http/simple_filter/config/config.pb.h"  
  
namespace Envoy {  
namespace Http {  
namespace SimpleFilter {  

Http::FilterFactoryCb SimpleFilterFactory::createFilterFactoryFromProto(  
    const Protobuf::Message&, const std::string&,  
    Server::Configuration::FactoryContext&) {  
  return [](Http::FilterChainFactoryCallbacks& callbacks) {  
    callbacks.addStreamFilter(std::make_shared<SimpleFilter>());  
  };  
}

ProtobufTypes::MessagePtr SimpleFilterFactory::createEmptyConfigProto() {  
  return std::make_unique<simple_filter::SimpleFilterConfig>();  // 使用自定义配置  
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