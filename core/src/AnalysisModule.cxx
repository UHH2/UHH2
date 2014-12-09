#include "UHH2/core/include/AnalysisModule.h"

using namespace uhh2;

AnalysisModule::~AnalysisModule(){}
Context::~Context(){}

void Context::fail(const std::string & key) const{
    throw std::runtime_error("did not find setting '" + key + "'");
}


/** \brief AnalysisModule which does nothing and lets all events pass
 * 
 * Only useful for testing.
 */
class NoopAnalysisModule: public AnalysisModule {
public:
    NoopAnalysisModule(Context & ctx){}
    virtual bool process(Event &){
        return true;
    }
};

UHH2_REGISTER_ANALYSIS_MODULE(NoopAnalysisModule)
