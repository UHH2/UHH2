#include "UHH2/common/include/MCLargeWeightKiller.h"
#include "UHH2/common/include/PartonHT.h"
#include "UHH2/common/include/Utils.h"

using namespace uhh2;


MCLargeWeightKiller::MCLargeWeightKiller(Context & ctx,
                                         float recoJetGenHTRatioMax,
                                         float genJetGenHTRatioMax,
                                         float recoJetQScaleRatioMax,
                                         float PUpTHatGenHTRatioMax,
                                         float recoJetpTHatRatioMax,
                                         float genJetpTHatRatioMax,
                                         const std::string jetsName,
                                         const std::string genjetsName):
    recoJetGenHTRatioMax_(recoJetGenHTRatioMax),
    genJetGenHTRatioMax_(genJetGenHTRatioMax),
    recoJetQScaleRatioMax_(recoJetQScaleRatioMax),
    PUpTHatGenHTRatioMax_(PUpTHatGenHTRatioMax),
    recoJetpTHatRatioMax_(recoJetpTHatRatioMax),
    genJetpTHatRatioMax_(genJetpTHatRatioMax),
    jetsHandle_(ctx.get_handle<std::vector<Jet>>(jetsName)),
    genjetsHandle_(ctx.get_handle<std::vector<GenJet>>(genjetsName))
{}

bool MCLargeWeightKiller::passes(const uhh2::Event & event) {

    if (event.isRealData) return true;

    float genHT = PartonHT::calculate(*event.genparticles);
    if (genHT == 0) {
        std::cout << "MCLargeWeightKiller Warning: Parton HT = 0, is this expected?" << std::endl;
    }

    auto recoJets = event.get(jetsHandle_);
    sort_by_pt(recoJets);
    bool hasRecoJets = recoJets.size() > 0;
    float recoJetPt = (hasRecoJets) ? recoJets[0].pt() : 0.;

    auto genJets = event.get(genjetsHandle_);
    sort_by_pt(genJets);
    bool hasGenJets = genJets.size() > 0;
    float genJetPt = (hasGenJets) ? genJets[0].pt() : 0.;

    // Check ratio of jet pT / gen HT
    if ((genHT > 0) && hasRecoJets && ((recoJetPt / genHT) > recoJetGenHTRatioMax_)) { return false; }
    if ((genHT > 0) && hasGenJets && ((genJetPt / genHT) > genJetGenHTRatioMax_)) { return false; }

    // Check ratio of jet pT / Q scale
    float qScale = event.genInfo->qScale();
    if ((qScale > 0) && hasRecoJets && ((recoJetPt / qScale) > recoJetQScaleRatioMax_)) { return false; }

    // Check ratio of pileup maximum pTHat / gen HT (i.e. is the scale of PU > hard event?)
    float PU_pThat = event.genInfo->PU_pT_hat_max();
    if ((genHT > 0) && ((PU_pThat / genHT) > PUpTHatGenHTRatioMax_)) { return false; }

    // Check event weight is sensible based on pthat - but isn't always available
    // e.g. exists for Pythia pT-binned, but not HT-binned
    if (event.genInfo->binningValues().size() > 0) {
        double ptHat = event.genInfo->binningValues().at(0); // yes this is correct. no idea why
        if ((ptHat > 0) && hasRecoJets && ((recoJetPt / ptHat) > recoJetpTHatRatioMax_)) return false;
        if ((ptHat > 0) && hasGenJets && ((genJetPt / ptHat) > genJetpTHatRatioMax_)) return false;
    }

    return true;
}
