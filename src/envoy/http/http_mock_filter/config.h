#pragma once  
  
#include "source/extensions/filters/http/common/factory_base.h"  
  
namespace Envoy {  
namespace Http {  
namespace HttpMockFilter {  
  
class HttpMockFilterFactory   
    : public Server::Configuration::NamedHttpFilterConfigFactory {  
public:  
  Http::FilterFactoryCb createFilterFactoryFromProto(  
      const Protobuf::Message& config,   
      const std::string& stat_prefix,  
      Server::Configuration::FactoryContext& context) override;  
  ProtobufTypes::MessagePtr createEmptyConfigProto() override;  
  std::string name() const override;  
};  
  
} // namespace HttpMockFilter  
} // namespace Http  
} // namespace Envoy