#include "UHH2/common/include/ElectronIds.h"

using namespace uhh2;

namespace {

// The values to cut on. Corresponds to the tables in the twiki:
// in the tables at https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
// following the same order of variables.
// The only difference to the twiki tables is the passes_conversion_rejection flag which replaces the 'Conversion rejection: vertex fit probability'
// and is treated as a boolean here.
struct ele_cutvalues {
    float abs_dEtaIn, abs_dPhiIn, full5x5_sigmaIetaIeta, HoverE, fabs_d0, fabs_dz, fabs_1oE_1op, pfiso_dbeta_dr03, passes_conversion_rejection, cr_misshits;
};

ele_cutvalues cv_from_ele(const Electron & ele, const Event & event){
    ele_cutvalues result;
    assert(event.pvs);
    if(event.pvs->size() == 0){
        result.abs_dEtaIn = std::numeric_limits<float>::infinity();
        return result;
    }
    const auto & pv = (*event.pvs)[0];
    result.abs_dEtaIn = std::abs(ele.dEtaIn());
    result.abs_dPhiIn = std::abs(ele.dPhiIn());
    result.full5x5_sigmaIetaIeta = ele.sigmaIEtaIEta();
    result.HoverE = ele.HoverE();
    result.fabs_d0 = ele.gsfTrack_dxy_vertex(pv.x(), pv.y());
    result.fabs_dz = ele.gsfTrack_dz_vertex(pv.x(), pv.y(), pv.z());
    result.fabs_1oE_1op = std::numeric_limits<float>::infinity();
    if(ele.EcalEnergy() > 0.0){
        result.fabs_1oE_1op = std::abs(1.0f/ele.EcalEnergy() - ele.EoverPIn()/ele.EcalEnergy());
    }
    result.pfiso_dbeta_dr03 = ele.relIsodb();
    result.cr_misshits = ele.gsfTrack_trackerExpectedHitsInner_numberOfLostHits();
    result.passes_conversion_rejection = ele.passconversionveto() ? 2.0f : 0.0f;
    return result;
}

bool passes_thresholds(const ele_cutvalues & vals, const ele_cutvalues & thresholds){
    if(vals.abs_dEtaIn >= thresholds.abs_dEtaIn) return false;
    if(vals.abs_dPhiIn >= thresholds.abs_dPhiIn) return false;
    if(vals.full5x5_sigmaIetaIeta >= thresholds.full5x5_sigmaIetaIeta) return false;
    if(vals.HoverE >= thresholds.HoverE) return false;
    if(vals.fabs_d0 >= thresholds.fabs_d0) return false;
    if(vals.fabs_dz >= thresholds.fabs_dz) return false;
    if(vals.fabs_1oE_1op >= thresholds.fabs_1oE_1op) return false;
    if(vals.pfiso_dbeta_dr03 >= thresholds.pfiso_dbeta_dr03) return false;
    // note: for passes conversion rejection, comparison is inverted
    if(vals.passes_conversion_rejection < thresholds.passes_conversion_rejection) return false;
    // note for missing hits: >, not >=
    if(vals.cr_misshits > thresholds.cr_misshits) return false;
    return true;
}

// check whether electron passes the given id, based on cut values given for barrel and endcap separately.
bool passes_id(const Electron & ele, const Event & event, const ele_cutvalues & thresh_barrel, const ele_cutvalues & thresh_endcap){
    auto abs_eta = std::abs(ele.supercluster_eta());
    if(abs_eta <= 1.479f){
        return passes_thresholds(cv_from_ele(ele, event), thresh_barrel);
    }
    else{
        return passes_thresholds(cv_from_ele(ele, event), thresh_endcap);
    }
}

}


bool ElectronID_CSA14_50ns_loose(const Electron & ele, const uhh2::Event & event){
    static const auto thresholds_barrel = ele_cutvalues{0.016f, 0.080f, 0.012f, 0.15f, 0.019f, 0.036f, 0.11f, 0.18f, 1.0f, 1.0f};
    static const auto thresholds_endcap = ele_cutvalues{0.025f, 0.097f, 0.032f, 0.12f, 0.099f, 0.880f, 0.11f, 0.21f, 1.0f, 1.0f};
    return passes_id(ele, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_CSA14_50ns_medium(const Electron & ele, const uhh2::Event & event){
    static const auto thresholds_barrel = ele_cutvalues{0.015f, 0.051f, 0.010f, 0.100f, 0.012f, 0.030f, 0.053f, 0.14f, 1.0f, 1.0f};
    static const auto thresholds_endcap = ele_cutvalues{0.023f, 0.056f, 0.030f, 0.099f, 0.068f, 0.780f, 0.110f, 0.15f, 1.0f, 1.0f};
    return passes_id(ele, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_CSA14_50ns_tight(const Electron & ele, const uhh2::Event & event){
    static const auto thresholds_barrel = ele_cutvalues{0.012f, 0.024f, 0.010f, 0.074f, 0.0091f, 0.017f, 0.026f, 0.10f, 1.0f, 1.0f};
    static const auto thresholds_endcap = ele_cutvalues{0.019f, 0.043f, 0.029f, 0.080f, 0.0370f, 0.065f, 0.076f, 0.14f, 1.0f, 1.0f};
    return passes_id(ele, event, thresholds_barrel, thresholds_endcap);
}
