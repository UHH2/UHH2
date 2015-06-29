#pragma once

#include "Tags.h"
#include "TLorentzVector.h"

class JetBTagInfo{
public:
    
  enum tag { /* for future use (more b-taggers, etc.) */ };
  
  JetBTagInfo(){
    //clear track impact parameter tag info
    m_TrackMomentum.clear();
    m_TrackEta.clear();
    m_TrackEtaRel.clear();
    m_TrackDeltaR.clear();
    m_TrackSip3dVal.clear();
    m_TrackSip3dSig.clear();
    m_TrackSip2dVal.clear();
    m_TrackSip2dSig.clear();
    m_TrackDecayLenVal.clear();
    m_TrackChi2.clear();
    m_TrackNTotalHits.clear();
    m_TrackNPixelHits.clear();
    m_TrackPtRel.clear();
    m_TrackPPar.clear();
    m_TrackPtRatio.clear();
    m_TrackPParRatio.clear();
    m_TrackJetDistVal.clear();
    m_TrackJetDistSig.clear();
    m_TrackGhostTrackDistVal.clear();
    m_TrackGhostTrackDistSig.clear();
    m_TrackGhostTrackWeight.clear();
  
    //clear secondary vertex tag info
    m_FlightDistance2dVal.clear();
    m_FlightDistance2dSig.clear();
    m_FlightDistance3dVal.clear();
    m_FlightDistance3dSig.clear();
    m_VertexJetDeltaR.clear();
    m_VertexNTracks.clear();
    m_SecondaryVertex.clear();
    m_VertexChi2.clear();                                
    m_VertexNdof.clear();                    
    m_VertexNormalizedChi2.clear();
  }

  //track impact parameter tag info getters
  std::vector<float> TrackMomentum() const{return m_TrackMomentum;}
  std::vector<float> TrackEta() const{return m_TrackEta;}
  std::vector<float> TrackEtaRel() const{return m_TrackEtaRel;}
  std::vector<float> TrackDeltaR() const{return m_TrackDeltaR;}
  std::vector<float> TrackSip3dVal() const{return m_TrackSip3dVal;}
  std::vector<float> TrackSip3dSig() const{return m_TrackSip3dSig;}
  std::vector<float> TrackSip2dVal() const{return m_TrackSip2dVal;}
  std::vector<float> TrackSip2dSig() const{return m_TrackSip2dSig;}
  std::vector<float> TrackDecayLenVal() const{return m_TrackDecayLenVal;}
  std::vector<float> TrackChi2() const{return m_TrackChi2;}
  std::vector<float> TrackNTotalHits() const{return m_TrackNTotalHits;}
  std::vector<float> TrackNPixelHits() const{return m_TrackNPixelHits;}
  std::vector<float> TrackPtRel() const{return m_TrackPtRel;}
  std::vector<float> TrackPPar() const{return m_TrackPPar;}
  std::vector<float> TrackPtRatio() const{return m_TrackPtRatio;}
  std::vector<float> TrackPParRatio() const{return m_TrackPParRatio;}
  std::vector<float> TrackJetDistVal() const{return m_TrackJetDistVal;}
  std::vector<float> TrackJetDistSig() const{return m_TrackJetDistSig;}
  std::vector<float> TrackGhostTrackDistVal() const{return m_TrackGhostTrackDistVal;}
  std::vector<float> TrackGhostTrackDistSig() const{return m_TrackGhostTrackDistSig;}
  std::vector<float> TrackGhostTrackWeight() const{return m_TrackGhostTrackWeight;}

  //secondary vertex tag info getters
  std::vector<float> FlightDistance2dVal() const{return m_FlightDistance2dVal;}
  std::vector<float> FlightDistance2dSig() const{return m_FlightDistance2dSig;}
  std::vector<float> FlightDistance3dVal() const{return m_FlightDistance3dVal;}
  std::vector<float> FlightDistance3dSig() const{return m_FlightDistance3dSig;}
  std::vector<float> VertexJetDeltaR() const{return m_VertexJetDeltaR;}
  int JetNSecondaryVertices() const{return m_JetNSecondaryVertices;}
  std::vector<int> VertexNTracks() const{return m_VertexNTracks;}
  std::vector<TLorentzVector > SecondaryVertex() const{return m_SecondaryVertex;}
  std::vector<float> VertexChi2() const{return m_VertexChi2;}
  std::vector<float> VertexNdof() const{return m_VertexNdof;}
  std::vector<float> VertexNormalizedChi2() const{return m_VertexNormalizedChi2;}
  //JTC SV variables
  int VertexCategoryJTC() const{return m_VertexCategoryJTC;}
  float VertexMassJTC() const{return m_VertexMassJTC;}
  float VertexEnergyRatioJTC() const{return m_VertexEnergyRatioJTC;}
  float TrackSip3dSigAboveCharmJTC() const{return m_TrackSip3dSigAboveCharmJTC;}

  //track impact parameter tag info setters
  void set_TrackMomentum(std::vector<float> t){ m_TrackMomentum=t;}
  void set_TrackEta(std::vector<float> t){ m_TrackEta=t;}
  void set_TrackEtaRel(std::vector<float> t){ m_TrackEtaRel=t;}
  void set_TrackDeltaR(std::vector<float> t){ m_TrackDeltaR=t;}
  void set_TrackSip3dVal(std::vector<float> t){ m_TrackSip3dVal=t;}
  void set_TrackSip3dSig(std::vector<float> t){ m_TrackSip3dSig=t;}
  void set_TrackSip2dVal(std::vector<float> t){ m_TrackSip2dVal=t;}
  void set_TrackSip2dSig(std::vector<float> t){ m_TrackSip2dSig=t;}
  void set_TrackDecayLenVal(std::vector<float> t){ m_TrackDecayLenVal=t;}
  void set_TrackChi2(std::vector<float> t){ m_TrackChi2=t;}
  void set_TrackNTotalHits(std::vector<float> t){ m_TrackNTotalHits=t;}
  void set_TrackNPixelHits(std::vector<float> t){ m_TrackNPixelHits=t;}
  void set_TrackPtRel(std::vector<float> t){ m_TrackPtRel=t;}
  void set_TrackPPar(std::vector<float> t){ m_TrackPPar=t;}
  void set_TrackPtRatio(std::vector<float> t){ m_TrackPtRatio=t;}
  void set_TrackPParRatio(std::vector<float> t){ m_TrackPParRatio=t;}
  void set_TrackJetDistVal(std::vector<float> t){ m_TrackJetDistVal=t;}
  void set_TrackJetDistSig(std::vector<float> t){ m_TrackJetDistSig=t;}
  void set_TrackGhostTrackDistVal(std::vector<float> t){ m_TrackGhostTrackDistVal=t;}
  void set_TrackGhostTrackDistSig(std::vector<float> t){ m_TrackGhostTrackDistSig=t;}
  void set_TrackGhostTrackWeight(std::vector<float> t){ m_TrackGhostTrackWeight=t;}

  //secondary vertex tag info setters
  void set_FlightDistance2dVal(std::vector<float> t){ m_FlightDistance2dVal=t;}
  void set_FlightDistance2dSig(std::vector<float> t){ m_FlightDistance2dSig=t;}
  void set_FlightDistance3dVal(std::vector<float> t){ m_FlightDistance3dVal=t;}
  void set_FlightDistance3dSig(std::vector<float> t){ m_FlightDistance3dSig=t;}
  void set_VertexJetDeltaR(std::vector<float> t){ m_VertexJetDeltaR=t;}
  void set_JetNSecondaryVertices(int t){ m_JetNSecondaryVertices=t;}
  void set_VertexNTracks(std::vector<int> t){ m_VertexNTracks=t;}
  void set_SecondaryVertex(std::vector<TLorentzVector > t){ m_SecondaryVertex=t;}
  void set_VertexChi2(std::vector<float> t){ m_VertexChi2=t;}
  void set_VertexNdof(std::vector<float> t){ m_VertexNdof=t;}
  void set_VertexNormalizedChi2(std::vector<float> t){ m_VertexNormalizedChi2=t;}
  //JTC SV variables
  void set_VertexCategoryJTC(int t){ m_VertexCategoryJTC=t;}
  void set_VertexMassJTC(float t){ m_VertexMassJTC=t;}
  void set_VertexEnergyRatioJTC(float t){ m_VertexEnergyRatioJTC=t;}
  void set_TrackSip3dSigAboveCharmJTC(float t){ m_TrackSip3dSigAboveCharmJTC=t;}

 private:
  
  //track impact parameter tag info
  std::vector<float> m_TrackMomentum;
  std::vector<float> m_TrackEta;
  std::vector<float> m_TrackEtaRel;
  std::vector<float> m_TrackDeltaR;
  std::vector<float> m_TrackSip3dVal;
  std::vector<float> m_TrackSip3dSig;
  std::vector<float> m_TrackSip2dVal;
  std::vector<float> m_TrackSip2dSig;
  std::vector<float> m_TrackDecayLenVal;
  std::vector<float> m_TrackChi2;
  std::vector<float> m_TrackNTotalHits;
  std::vector<float> m_TrackNPixelHits;
  std::vector<float> m_TrackPtRel;
  std::vector<float> m_TrackPPar;
  std::vector<float> m_TrackPtRatio;
  std::vector<float> m_TrackPParRatio;
  std::vector<float> m_TrackJetDistVal;
  std::vector<float> m_TrackJetDistSig;
  std::vector<float> m_TrackGhostTrackDistVal;
  std::vector<float> m_TrackGhostTrackDistSig;
  std::vector<float> m_TrackGhostTrackWeight;
  
  //secondary vertex tag info
  std::vector<float> m_FlightDistance2dVal;
  std::vector<float> m_FlightDistance2dSig;
  std::vector<float> m_FlightDistance3dVal;
  std::vector<float> m_FlightDistance3dSig;
  std::vector<float> m_VertexJetDeltaR;
  int m_JetNSecondaryVertices;
  std::vector<int> m_VertexNTracks;
  std::vector<TLorentzVector> m_SecondaryVertex;
  std::vector<float> m_VertexChi2;                                
  std::vector<float> m_VertexNdof;                    
  std::vector<float> m_VertexNormalizedChi2;
  //JTC SV variables
  int m_VertexCategoryJTC;
  float m_VertexMassJTC;
  float m_VertexEnergyRatioJTC;
  float m_TrackSip3dSigAboveCharmJTC;
  
  Tags tags;
};

