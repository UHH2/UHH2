#pragma once

#include "FlavorParticle.h"
#include "Tags.h"
#include "TLorentzVector.h"

class JetBTagInfo : public FlavorParticle{
public:
    
  enum tag { /* for future use (more b-taggers, etc.) */ };
  
  JetBTagInfo(){
    //clear track impact parameter tag info
    m_subTrackMomentum.clear();
    m_subTrackEta.clear();
    m_subTrackEtaRel.clear();
    m_subTrackDeltaR.clear();
    m_subTrackSip3dVal.clear();
    m_subTrackSip3dSig.clear();
    m_subTrackSip2dVal.clear();
    m_subTrackSip2dSig.clear();
    m_subTrackDecayLenVal.clear();
    m_subTrackChi2.clear();
    m_subTrackNTotalHits.clear();
    m_subTrackNPixelHits.clear();
    m_subTrackPtRel.clear();
    m_subTrackPPar.clear();
    m_subTrackPtRatio.clear();
    m_subTrackPParRatio.clear();
    m_subTrackJetDistVal.clear();
    m_subTrackJetDistSig.clear();
    m_subTrackGhostTrackDistVal.clear();
    m_subTrackGhostTrackDistSig.clear();
    m_subTrackGhostTrackWeight.clear();
  
    //clear secondary vertex tag info
    m_subFlightDistance2dVal.clear();
    m_subFlightDistance2dSig.clear();
    m_subFlightDistance3dVal.clear();
    m_subFlightDistance3dSig.clear();
    m_subVertexJetDeltaR.clear();
    m_subJetNSecondaryVertices.clear();
    m_subVertexNTracks.clear();
    m_subSecondaryVertex.clear();
    m_subVertexChi2.clear();                                
    m_subVertexNdof.clear();                    
    m_subVertexNormalizedChi2.clear();
    m_subVertexTracksSize.clear();
    //JTC SV variables
    m_subVertexCategoryJTC.clear();
    m_subVertexMassJTC.clear();
    m_subVertexEnergyRatioJTC.clear();
    m_subTrackSip3dSigAboveCharmJTC.clear();

  }

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
  
  Tags tags;
};

