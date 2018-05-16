#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/common/include/JetCorrections.h"

using namespace uhh2;
using namespace std;

class TestJetCorrections: public uhh2::AnalysisModule {
public:
    explicit TestJetCorrections(Context &ctx ) {
        jec_mc.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_L123_AK4PFchs_MC));
	jec_data.reset(new JetCorrector(ctx, JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA));
        printer_before.reset(new JetPrinter("before", 20.0));
        printer_after.reset(new JetPrinter("after", 20.0));
    }
    
    virtual bool process(Event & e) override;
    
private:
    std::unique_ptr<AnalysisModule> jec_mc, jec_data;
    std::unique_ptr<AnalysisModule> printer_before, printer_after;
};

namespace {

// return value: 0 = exactly the same pt, 1 = almost the same pt (rel. pt diff < 1e-3), 2 = not the same pt, 3 = jet vectors do not have the same size
int compare_jets(const vector<Jet> & jets1, const vector<Jet> & jets2){
    if(jets1.size()!= jets2.size()) return 3;
    const size_t njets = jets1.size();
    float max_reldiff = 0.0f;
    for(size_t i=0; i<njets; ++i){
        float reldiff = fabs(jets1[i].pt() - jets2[i].pt()) / jets1[i].pt();
        max_reldiff = std::max(reldiff, max_reldiff);
    }
    if(max_reldiff == 0.0f) return 0;
    else if(max_reldiff < 1e-3f) return 1;
    else return 2;
}

    
}


bool TestJetCorrections::process(Event & e){
    if(e.jets->empty()) return true;
    // NOTE: we test here only that the jet correction module does 'something' to the jets which looks reasonable;
    // the size of the corrections are not tested, as this is a matter to verify centrally in CMS
    auto jets_before = *e.jets;
    printer_before->process(e);
    jec_mc->process(e);
    printer_after->process(e);
    // re-applying MC jet energy corrections should not change anything:
    auto jets_after_mc_jec = *e.jets;
    assert(compare_jets(jets_before, jets_after_mc_jec) <= 2);
    jec_data->process(e);
    auto jets_after_data_jec = *e.jets;
    // applying data jet energy corrections on MC should change the pt considerably, but not the number of jets:
    assert(compare_jets(jets_before, jets_after_data_jec) == 2);
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(TestJetCorrections)


class TestJERSmear: public uhh2::AnalysisModule {
public:
    explicit TestJERSmear(Context & ctx) {
        jer.reset(new JetResolutionSmearer(ctx));
        printer_before.reset(new JetPrinter("before", 20.0));
        printer_after.reset(new JetPrinter("after", 20.0));
    }
    
    virtual bool process(Event & e) override {
        printer_before->process(e);
        auto jets_before = *e.jets;
        jer->process(e);
        printer_after->process(e);
        assert(compare_jets(jets_before, *e.jets) <= 2);
        return true;
    }
    
private:
    std::unique_ptr<AnalysisModule> jer, printer_before, printer_after;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestJERSmear)


class TestSDMassCalc: public uhh2::AnalysisModule {
public:
    explicit TestSDMassCalc(Context & ctx) {
        massCalcUncorr.reset(new SoftDropMassCalculator(ctx, false, ""));
        massCalcCorr.reset(new SoftDropMassCalculator(ctx, true, "common/data/puppiCorr.root"));
    }
    void print_topjets(const std::vector<TopJet> & topjets) {
        for (auto & jet : topjets){
            cout << "pt = " << jet.pt() << " eta = " << jet.eta() << " # subjets = " << jet.subjets().size() << " SD mass = " << jet.softdropmass() << endl;
        }
    }

    virtual bool process(Event & e) override {
        cout << "TopJets existing mSD:" << endl;
        print_topjets(*e.topjets);
        massCalcUncorr->process(e);
        cout << "TopJets new mSD, no corrections:" << endl;
        print_topjets(*e.topjets);
        massCalcCorr->process(e);
        cout << "TopJets new mSD, with corrections:" << endl;
        print_topjets(*e.topjets);

        return true;
    }
    
private:
    std::unique_ptr<AnalysisModule> massCalcUncorr, massCalcCorr;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestSDMassCalc)

