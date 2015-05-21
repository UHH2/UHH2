#pragma once

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "UHH2/core/include/Event.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"


namespace uhh2 {

class Context;

/** \brief Base class for a module converting a cmssw format to the uhh2 ntuple format
 *
 * Typically, each module has one input from CMSSW (one InputTag) and one output in the
 * uhh2::Event. The NtupleWriterModule::Config reflects that situation. In case
 * there are more inputs or more outputs, the module can still be used, but the Config
 * has to be changed accordingly.
 */
class NtupleWriterModule {
public:

    struct Config {
        uhh2::Context & ctx;
        edm::ConsumesCollector cc;
        edm::InputTag src;
        std::string dest; // name of output in uhh2::Event
        std::string dest_branchname; // name of output branch, usually the same as dest

        // cuts on the converted particles. Note that not all sub-modules
        // implement this cut.
        double ptmin = 0.0;
        double etamax = 1e10;

        Config(uhh2::Context & ctx_, edm::ConsumesCollector && cc_, const edm::InputTag & src_, const std::string & dest_,
                const std::string & dest_branchname_ = ""):
            ctx(ctx_), cc(std::move(cc_)), src(src_), dest(dest_), dest_branchname(dest_branchname_){
            if(dest_branchname.empty()){
                dest_branchname = dest;
            }
        }
    };

    // transfer content from the edm event to the uhh2 event:
    virtual void process(const edm::Event &, uhh2::Event &) = 0;
    virtual ~NtupleWriterModule();
};

}

