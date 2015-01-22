#ifndef TopJet_H
#define TopJet_H

#include "Jet.h"
#include "TLorentzVector.h"


/**
 *  @short top-jet class
 *  @author Thomas Peiffer
 */

class TopJet : public Jet{
 public:
  TopJet(){
    // initialize substructure variables to -1
    m_qjets_volatility = -1.; 
    m_tau1 = -1.;
    m_tau2 = -1.;
    m_tau3 = -1.;
  }

  explicit TopJet(const Jet & j){
    this->set_v4(j.v4());
    this->set_charge(j.charge());
    this->set_pdgId(j.pdgId());
    this->set_nTracks(j.nTracks());
    this->set_jetArea(j.jetArea());
    this->set_numberOfDaughters(j.numberOfDaughters());
    this->set_neutralEmEnergyFraction(j.neutralEmEnergyFraction());
    this->set_neutralHadronEnergyFraction(j.neutralHadronEnergyFraction());
    this->set_chargedEmEnergyFraction(j.chargedEmEnergyFraction());
    this->set_chargedHadronEnergyFraction(j.chargedHadronEnergyFraction());
    this->set_muonEnergyFraction(j.muonEnergyFraction());
    this->set_photonEnergyFraction(j.photonEnergyFraction());
    this->set_chargedMultiplicity(j.chargedMultiplicity());
    this->set_neutralMultiplicity(j.neutralMultiplicity());
    this->set_muonMultiplicity(j.muonMultiplicity());
    this->set_electronMultiplicity(j.electronMultiplicity());
    this->set_photonMultiplicity(j.photonMultiplicity());
    this->set_btag_simpleSecondaryVertexHighEff(j.btag_simpleSecondaryVertexHighEff());
    this->set_btag_simpleSecondaryVertexHighPur(j.btag_simpleSecondaryVertexHighPur());
    this->set_btag_combinedSecondaryVertex(j.btag_combinedSecondaryVertex());
    this->set_btag_combinedSecondaryVertexMVA(j.btag_combinedSecondaryVertexMVA());
    this->set_btag_jetBProbability(j.btag_jetBProbability());
    this->set_btag_jetProbability(j.btag_jetProbability());
    this->set_JEC_factor_raw(j.JEC_factor_raw());
    this->set_genjet_index(j.genjet_index());
    std::vector<unsigned int> indices = j.pfconstituents_indices();
    for(unsigned int i=0; i<indices.size(); i++){
      this->add_pfconstituents_index(indices[i]);
    }
  }

  // substructure getters
  float qjets_volatility() const {return m_qjets_volatility;}
  float tau1() const {return m_tau1;}
  float tau2() const {return m_tau2;}
  float tau3() const {return m_tau3;}

  //btag variables getters
  std::vector<Particle> subjets() const{return m_subjets;}
  std::vector<float> subCSV() const{return m_subCSV;}
  std::vector<float> subArea() const{return m_subArea;}
  std::vector<float> subJEC_raw() const{return m_subJEC_raw;}
  std::vector<int> subFlavour() const{return m_subFlavour;}
  std::vector<float> btagsub_combinedSecondaryVertex() const{return m_subCSV;}
  std::vector<int> flavorsub() const{return m_subFlavour;}

  //track impact parameter tag info getters
  std::vector<std::vector<float> > subTrackMomentum() const{return m_subTrackMomentum;}
  std::vector<std::vector<float> > subTrackEta() const{return m_subTrackEta;}
  std::vector<std::vector<float> > subTrackEtaRel() const{return m_subTrackEtaRel;}
  std::vector<std::vector<float> > subTrackDeltaR() const{return m_subTrackDeltaR;}
  std::vector<std::vector<float> > subTrackSip3dVal() const{return m_subTrackSip3dVal;}
  std::vector<std::vector<float> > subTrackSip3dSig() const{return m_subTrackSip3dSig;}
  std::vector<std::vector<float> > subTrackSip2dVal() const{return m_subTrackSip2dVal;}
  std::vector<std::vector<float> > subTrackSip2dSig() const{return m_subTrackSip2dSig;}
  std::vector<std::vector<float> > subTrackDecayLenVal() const{return m_subTrackDecayLenVal;}
  std::vector<std::vector<float> > subTrackChi2() const{return m_subTrackChi2;}
  std::vector<std::vector<float> > subTrackNTotalHits() const{return m_subTrackNTotalHits;}
  std::vector<std::vector<float> > subTrackNPixelHits() const{return m_subTrackNPixelHits;}
  std::vector<std::vector<float> > subTrackPtRel() const{return m_subTrackPtRel;}
  std::vector<std::vector<float> > subTrackPPar() const{return m_subTrackPPar;}
  std::vector<std::vector<float> > subTrackPtRatio() const{return m_subTrackPtRatio;}
  std::vector<std::vector<float> > subTrackPParRatio() const{return m_subTrackPParRatio;}
  std::vector<std::vector<float> > subTrackJetDistVal() const{return m_subTrackJetDistVal;}
  std::vector<std::vector<float> > subTrackJetDistSig() const{return m_subTrackJetDistSig;}
  std::vector<std::vector<float> > subTrackGhostTrackDistVal() const{return m_subTrackGhostTrackDistVal;}
  std::vector<std::vector<float> > subTrackGhostTrackDistSig() const{return m_subTrackGhostTrackDistSig;}
  std::vector<std::vector<float> > subTrackGhostTrackWeight() const{return m_subTrackGhostTrackWeight;}

  //secondary vertex tag info getters
  std::vector<std::vector<float> > subFlightDistance2dVal() const{return m_subFlightDistance2dVal;}
  std::vector<std::vector<float> > subFlightDistance2dSig() const{return m_subFlightDistance2dSig;}
  std::vector<std::vector<float> > subFlightDistance3dVal() const{return m_subFlightDistance3dVal;}
  std::vector<std::vector<float> > subFlightDistance3dSig() const{return m_subFlightDistance3dSig;}
  std::vector<std::vector<float> > subVertexJetDeltaR() const{return m_subVertexJetDeltaR;}
  std::vector<float> subJetNSecondaryVertices() const{return m_subJetNSecondaryVertices;}
  std::vector<float> subVertexNTracks() const{return m_subVertexNTracks;}
  //std::vector<std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > > subSecondaryVertex() const{return m_subSecondaryVertex;}
  std::vector<std::vector<TLorentzVector > > subSecondaryVertex() const{return m_subSecondaryVertex;}
  std::vector<std::vector<float> > subVertexChi2() const{return m_subVertexChi2;}
  std::vector<std::vector<float> > subVertexNdof() const{return m_subVertexNdof;}
  std::vector<std::vector<float> > subVertexNormalizedChi2() const{return m_subVertexNormalizedChi2;}
  std::vector<std::vector<float> > subVertexTracksSize() const{return m_subVertexTracksSize;}
  //JTC SV variables
  std::vector<int> subVertexCategoryJTC() const{return m_subVertexCategoryJTC;}
  std::vector<float> subVertexMassJTC() const{return m_subVertexMassJTC;}
  std::vector<float> subVertexEnergyRatioJTC() const{return m_subVertexEnergyRatioJTC;}
  std::vector<float> subTrackSip3dSigAboveCharmJTC() const{return m_subTrackSip3dSigAboveCharmJTC;}

  // substructure setters
  void set_qjets_volatility(float x){m_qjets_volatility = x;}
  void set_tau1(float x){m_tau1 = x;}
  void set_tau2(float x){m_tau2 = x;}
  void set_tau3(float x){m_tau3 = x;}

  //btag variables setters
  void add_subjet(Particle p){m_subjets.push_back(p);}
  void add_subCSV(float discriminant){m_subCSV.push_back(discriminant);}
  void add_subArea(float area){m_subArea.push_back(area);}
  void add_subJEC_raw(float jecraw){m_subJEC_raw.push_back(jecraw);}
  void add_subFlavour(int flavour){m_subFlavour.push_back(flavour);}

  //remove subjets
  void rm_subjets(){m_subjets.clear();}
  void rm_JEC(){m_subJEC_raw.clear();}

  //track impact parameter tag info setters
  void add_subTrackMomentum(std::vector<float> t){ m_subTrackMomentum.push_back(t);}
  void add_subTrackEta(std::vector<float> t){ m_subTrackEta.push_back(t);}
  void add_subTrackEtaRel(std::vector<float> t){ m_subTrackEtaRel.push_back(t);}
  void add_subTrackDeltaR(std::vector<float> t){ m_subTrackDeltaR.push_back(t);}
  void add_subTrackSip3dVal(std::vector<float> t){ m_subTrackSip3dVal.push_back(t);}
  void add_subTrackSip3dSig(std::vector<float> t){ m_subTrackSip3dSig.push_back(t);}
  void add_subTrackSip2dVal(std::vector<float> t){ m_subTrackSip2dVal.push_back(t);}
  void add_subTrackSip2dSig(std::vector<float> t){ m_subTrackSip2dSig.push_back(t);}
  void add_subTrackDecayLenVal(std::vector<float> t){ m_subTrackDecayLenVal.push_back(t);}
  void add_subTrackChi2(std::vector<float> t){ m_subTrackChi2.push_back(t);}
  void add_subTrackNTotalHits(std::vector<float> t){ m_subTrackNTotalHits.push_back(t);}
  void add_subTrackNPixelHits(std::vector<float> t){ m_subTrackNPixelHits.push_back(t);}
  void add_subTrackPtRel(std::vector<float> t){ m_subTrackPtRel.push_back(t);}
  void add_subTrackPPar(std::vector<float> t){ m_subTrackPPar.push_back(t);}
  void add_subTrackPtRatio(std::vector<float> t){ m_subTrackPtRatio.push_back(t);}
  void add_subTrackPParRatio(std::vector<float> t){ m_subTrackPParRatio.push_back(t);}
  void add_subTrackJetDistVal(std::vector<float> t){ m_subTrackJetDistVal.push_back(t);}
  void add_subTrackJetDistSig(std::vector<float> t){ m_subTrackJetDistSig.push_back(t);}
  void add_subTrackGhostTrackDistVal(std::vector<float> t){ m_subTrackGhostTrackDistVal.push_back(t);}
  void add_subTrackGhostTrackDistSig(std::vector<float> t){ m_subTrackGhostTrackDistSig.push_back(t);}
  void add_subTrackGhostTrackWeight(std::vector<float> t){ m_subTrackGhostTrackWeight.push_back(t);}

  //secondary vertex tag info setters
  void add_subFlightDistance2dVal(std::vector<float> t){ m_subFlightDistance2dVal.push_back(t);}
  void add_subFlightDistance2dSig(std::vector<float> t){ m_subFlightDistance2dSig.push_back(t);}
  void add_subFlightDistance3dVal(std::vector<float> t){ m_subFlightDistance3dVal.push_back(t);}
  void add_subFlightDistance3dSig(std::vector<float> t){ m_subFlightDistance3dSig.push_back(t);}
  void add_subVertexJetDeltaR(std::vector<float> t){ m_subVertexJetDeltaR.push_back(t);}
  void add_subJetNSecondaryVertices(float t){ m_subJetNSecondaryVertices.push_back(t);}
  void add_subVertexNTracks(float t){ m_subVertexNTracks.push_back(t);}
  //void add_subSecondaryVertex(std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > t){ m_subSecondaryVertex.push_back(t);}
  void add_subSecondaryVertex(std::vector<TLorentzVector > t){ m_subSecondaryVertex.push_back(t);}
  void add_subVertexChi2(std::vector<float> t){ m_subVertexChi2.push_back(t);}
  void add_subVertexNdof(std::vector<float> t){ m_subVertexNdof.push_back(t);}
  void add_subVertexNormalizedChi2(std::vector<float> t){ m_subVertexNormalizedChi2.push_back(t);}
  void add_subVertexTracksSize(std::vector<float> t){ m_subVertexTracksSize.push_back(t);}
  //JTC SV variables
  void add_subVertexCategoryJTC(int t){ m_subVertexCategoryJTC.push_back(t);}
  void add_subVertexMassJTC(float t){ m_subVertexMassJTC.push_back(t);}
  void add_subVertexEnergyRatioJTC(float t){ m_subVertexEnergyRatioJTC.push_back(t);}
  void add_subTrackSip3dSigAboveCharmJTC(float t){ m_subTrackSip3dSigAboveCharmJTC.push_back(t);}


 private:
  std::vector<Particle> m_subjets;

  // substructure information
  float m_qjets_volatility;
  float m_tau1;
  float m_tau2;
  float m_tau3;

  //btag variables
  std::vector<float> m_subCSV;
  std::vector<int> m_subFlavour;
  std::vector<float> m_subArea;
  std::vector<float> m_subJEC_raw;

  //track impact parameter tag info
  std::vector<std::vector<float> > m_subTrackMomentum;
  std::vector<std::vector<float> > m_subTrackEta;
  std::vector<std::vector<float> > m_subTrackEtaRel;
  std::vector<std::vector<float> > m_subTrackDeltaR;
  std::vector<std::vector<float> > m_subTrackSip3dVal;
  std::vector<std::vector<float> > m_subTrackSip3dSig;
  std::vector<std::vector<float> > m_subTrackSip2dVal;
  std::vector<std::vector<float> > m_subTrackSip2dSig;
  std::vector<std::vector<float> > m_subTrackDecayLenVal;
  std::vector<std::vector<float> > m_subTrackChi2;
  std::vector<std::vector<float> > m_subTrackNTotalHits;
  std::vector<std::vector<float> > m_subTrackNPixelHits;
  std::vector<std::vector<float> > m_subTrackPtRel;
  std::vector<std::vector<float> > m_subTrackPPar;
  std::vector<std::vector<float> > m_subTrackPtRatio;
  std::vector<std::vector<float> > m_subTrackPParRatio;
  std::vector<std::vector<float> > m_subTrackJetDistVal;
  std::vector<std::vector<float> > m_subTrackJetDistSig;
  std::vector<std::vector<float> > m_subTrackGhostTrackDistVal;
  std::vector<std::vector<float> > m_subTrackGhostTrackDistSig;
  std::vector<std::vector<float> > m_subTrackGhostTrackWeight;
  
  //secondary vertex tag info
  std::vector<std::vector<float> > m_subFlightDistance2dVal;
  std::vector<std::vector<float> > m_subFlightDistance2dSig;
  std::vector<std::vector<float> > m_subFlightDistance3dVal;
  std::vector<std::vector<float> > m_subFlightDistance3dSig;
  std::vector<std::vector<float> > m_subVertexJetDeltaR;
  std::vector<float> m_subJetNSecondaryVertices;
  std::vector<float> m_subVertexNTracks;

  //std::vector<std::vector< ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > > m_subSecondaryVertex;
  std::vector<std::vector<TLorentzVector> > m_subSecondaryVertex;
  std::vector<std::vector<float> > m_subVertexChi2;                                
  std::vector<std::vector<float> > m_subVertexNdof;                    
  std::vector<std::vector<float> > m_subVertexNormalizedChi2;
  std::vector<std::vector<float> > m_subVertexTracksSize;
  //JTC SV variables
  std::vector<int> m_subVertexCategoryJTC;
  std::vector<float> m_subVertexMassJTC;
  std::vector<float> m_subVertexEnergyRatioJTC;
  std::vector<float> m_subTrackSip3dSigAboveCharmJTC;

};

#endif
