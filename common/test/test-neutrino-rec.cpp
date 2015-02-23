#include <boost/test/unit_test.hpp>
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/common/include/TTbarReconstruction.h"
#include <random>

#include "Math/Rotation3D.h"
#include "Math/Quaternion.h"

using namespace std;
using namespace uhh2;

namespace {

// generate back-to-back decays from a massive object into two
// massless objects. Will first create the decay along the
// x-axis, and then rotate and boost the system, if requested.
class WDecayGenerator {
public:
    WDecayGenerator(float mW_, bool do_rotate_, bool do_boost_): mW(mW_), do_rotate(do_rotate_), do_boost(do_boost_){}
    
    std::pair<LorentzVectorXYZE, LorentzVectorXYZE> generate() { 
        // back-to-back in x direction:
        LorentzVectorXYZE p1(mW / 2, 0, 0, mW / 2);
        LorentzVectorXYZE p2(-mW / 2, 0, 0, mW / 2);
        if(do_rotate){
            // create a random rotation by using a random unit quaternion:
            auto Q0 = uni(rnd), Q1 = uni(rnd), Q2 = uni(rnd), Q3 = uni(rnd);
            auto QNorm = sqrt(Q0*Q0 + Q1*Q1 + Q2*Q2 + Q3*Q3);
            ROOT::Math::Rotation3D rot(ROOT::Math::Quaternion(Q0 / QNorm, Q1 / QNorm, Q2 / QNorm, Q3 / QNorm));
            //cout << "before rot: " << p1 << endl;
            p1 = rot * p1;
            //cout << "after rot: " << p1 << endl;
            
            p2 = rot * p2;
        }
        return make_pair(p1, p2);
    }
    
private:
    float mW;
    bool do_rotate, do_boost;
    mt19937_64 rnd;
    uniform_real_distribution<double> uni;
};

// mW as used in the reconstruction:
constexpr float mW_rec = 80.399f;
    
}

BOOST_AUTO_TEST_SUITE(neutrino_rec)

BOOST_AUTO_TEST_CASE(trivial) {
    // note: use a slightly smaller W mass than the reconstruction, such that 2 solutions exist.
    WDecayGenerator wdec(80.3f, false, false);
    auto lep_neu = wdec.generate();
    auto lepton = lep_neu.first;
    auto neutrino = lep_neu.second;
    
    auto solutions = NeutrinoReconstruction(toPtEtaPhi(lepton), toPtEtaPhi(neutrino));
    BOOST_REQUIRE_EQUAL(solutions.size(), 2u);
    BOOST_CHECK_LT(fabs(solutions[0].M2()), 0.1);
    BOOST_CHECK_LT(fabs(solutions[1].M2()), 0.1);
    // make sure solutions fulfill W mass constraint:
    float mw1 = (solutions[0] + lepton).M();
    float mw2 = (solutions[1] + lepton).M();
    
    BOOST_CHECK(fabs(mw1 - mW_rec) < 1e-2f);
    BOOST_CHECK(fabs(mw2 - mW_rec) < 1e-2f);
}

BOOST_AUTO_TEST_CASE(no_real_solution) {
    // use a higher mass than the actual W mass, such that no solution exists:
    WDecayGenerator wdec(81.f, false, false);
    auto lep_neu = wdec.generate();
    auto lepton = lep_neu.first;
    auto neutrino = lep_neu.second;
    auto solutions = NeutrinoReconstruction(toPtEtaPhi(lepton), toPtEtaPhi(neutrino));
    BOOST_REQUIRE_EQUAL(solutions.size(), 1u);
    BOOST_CHECK_LT(fabs(solutions[0].M2()), 0.1);
    
    // but should still be close to original mass:
    float mw1 = (solutions[0] + lepton).M();
    BOOST_CHECK(fabs(mw1 - 81.f) < 1e-2f);
}

BOOST_AUTO_TEST_CASE(rotation) {
    // note: use a slightly smaller W mass than the reconstruction, such that 2 solutions exist.
    WDecayGenerator wdec(80.3f, true, false);
    
    for(int i=0; i<100; ++i){
        auto lep_neu = wdec.generate();
        auto lepton = lep_neu.first;
        auto neutrino = lep_neu.second;
    
        auto solutions = NeutrinoReconstruction(toPtEtaPhi(lepton), toPtEtaPhi(neutrino));
        
        BOOST_REQUIRE_EQUAL(solutions.size(), 2u);
        BOOST_CHECK_LT(fabs(solutions[0].M2()), 0.1);
        BOOST_CHECK_LT(fabs(solutions[1].M2()), 0.1);
        // make sure solutions fulfill W mass constraint:
        float mw1 = (solutions[0] + lepton).M();
        float mw2 = (solutions[1] + lepton).M();
    
        BOOST_CHECK(fabs(mw1 - mW_rec) < 1e-2f);
        BOOST_CHECK(fabs(mw2 - mW_rec) < 1e-2f);
    }
}

BOOST_AUTO_TEST_CASE(rotation_noreal) {
    // use a large W mass so that not always solution exist
    WDecayGenerator wdec(85.f, true, false);
    
    for(int i=0; i<100; ++i){
        auto lep_neu = wdec.generate();
        auto lepton = lep_neu.first;
        auto neutrino = lep_neu.second;
        
        float mtw = sqrt(2 * lepton.Et() * neutrino.Et() * (1 - cos(deltaPhi(lepton, neutrino))));
    
        auto solutions = NeutrinoReconstruction(toPtEtaPhi(lepton), toPtEtaPhi(neutrino));
        
        // ignore cases in which mt is always the same as the assume w mass:
        if(fabs(mtw  - mW_rec) < 0.01f) continue;
        
        if(mtw > mW_rec){
            BOOST_REQUIRE_EQUAL(solutions.size(), 1u);
            BOOST_CHECK_LT(fabs(solutions[0].M2()), 0.1);
            
            // W mass constraint cannot be fulfilled, but should be close to Mtw now:
            float mw1 = (solutions[0] + lepton).M();
            BOOST_CHECK_LT(fabs(mw1 - mtw) / mtw , 1e-3f);
        }
        else{
            BOOST_REQUIRE_EQUAL(solutions.size(), 2u);
            BOOST_CHECK_LT(fabs(solutions[0].M2()), 0.1);
            BOOST_CHECK_LT(fabs(solutions[1].M2()), 0.1);
            
            float mw1 = (solutions[0] + lepton).M();
            float mw2 = (solutions[1] + lepton).M();
    
            BOOST_CHECK_LT(fabs(mw1 - mW_rec), 1e-2f);
            BOOST_CHECK_LT(fabs(mw2 - mW_rec), 1e-2f);
        }
    }
}


BOOST_AUTO_TEST_SUITE_END()

