#include "UHH2/core/include/AnalysisModule.h"
#include <stdexcept>

using namespace uhh2;

AnalysisModule::~AnalysisModule(){}

void AnalysisModule::endInputData(){}


Context::Context(GenericEventStructure & ges_): ges(ges_) {}

Context::~Context(){}

void Context::fail(const std::string & key) const{
    throw std::runtime_error("did not find setting '" + key + "'");
}

void Context::set_metadata(const std::string & name, const std::string & value, bool force){
    if(name.empty()){
        throw std::invalid_argument("Context::set_metadata: empty name not allowed");
    }
    if(name.find("===") != std::string::npos || name[name.size() - 1] == '='){
        throw std::invalid_argument("Context::set_metadata: tried to set metadata with name '" + name + "', but '===' / ending with '=' is not allowed");
    }
    if(value.find_first_of("\n\r\0") != std::string::npos){
        throw std::invalid_argument("Context::set_metadata: tried to set metadata with name '" + name + "' to a value containing \\r or \\n or \\0: '" + value + "'");
    }
    std::string key = "meta_" + name;
    auto it = settings.find(key);
    if(it != settings.end()){
        if(it->second == value) return; // nothing to do: value is the same.
        // update value only in case of force:
        if(force){
            it->second = value;
        }
        else{
            throw std::runtime_error("Context::set_metadata: Tried to overwrite metadata '" + name + "' with value '" + value + "' (current value: '" + it->second + "'). This is only allowed with force=true");
        }
    }
    else{
        settings[key] = value;
    }
}

/** \brief AnalysisModule which does nothing and lets all events pass
 *
 * Only useful for testing.
 */
class NoopAnalysisModule: public AnalysisModule {
public:
    NoopAnalysisModule(Context &){}
    virtual bool process(Event &){
        return true;
    }
    virtual void endInputData(){}
};

UHH2_REGISTER_ANALYSIS_MODULE(NoopAnalysisModule)
