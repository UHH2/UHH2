#pragma once

#include "UHH2/core/include/GenParticle.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

#include <vector>


/** \brief ttbar generator truth information
 * 
 * Interpret a vector of GenParticle as ttbar event, providing easier access to
 * the various components in the ttbar decay chain.
 * 
 * The class can either be used directly by passing the genparticles vector;
 * another option is to use the TTbarGenProducer (see below), which writes the
 * result to the event container, where it can be accessed later.
 */
class TTbarGen {
public:
  
  /** construct from genparticles.
   * 
   * The event should be an actual ttbar event, i.e.:
   *   - there are exactly two tops: one top and one anti-top
   *   - each top must have exactl two daughters
   *   - one of the top daughters must be a W
   *   - the Ws must have exactly two daughters
   * 
   * Note that it is allowed that more particles than those belonging to the ttbar system
   * are in genparts; those are ignored.
   * 
   * In case one of these conditions is not fulfilled, the behavior
   * depends on the throw_on_failure parameter: if it is true (the default), a runtime_error
   * is thrown with an explanation what failed. If it is false, no exception is thrown, but
   * not all contents is valid; most will return a 0 vector. The one thing guaranteed is that the
   * decaychannel will be e_notfound. If using throw_on_failure = false, it is thus a good idea
   * to check the decaychannel.
   */
  explicit TTbarGen(const std::vector<GenParticle> & genparts, bool throw_on_failure = true);

  enum E_DecayChannel{
    e_had, 
    e_ehad, 
    e_muhad,
    e_tauhad,
    e_ee,  
    e_mumu,
    e_tautau,
    e_emu,
    e_etau,
    e_mutau,
    e_notfound
  };

  GenParticle Top() const;
  GenParticle Antitop() const;
  GenParticle WTop() const;
  GenParticle WAntitop() const;
  GenParticle bTop() const;
  GenParticle bAntitop() const;
  GenParticle Wdecay1() const;
  GenParticle Wdecay2() const;
  GenParticle WMinusdecay1() const;
  GenParticle WMinusdecay2() const;
  E_DecayChannel DecayChannel() const;

  bool IsTopHadronicDecay() const;
  bool IsAntiTopHadronicDecay() const;
  bool IsSemiLeptonicDecay() const; 

  //only for l+jets decays
  GenParticle TopLep() const; 
  GenParticle TopHad() const;
  GenParticle BLep() const;
  GenParticle BHad() const; 
  GenParticle WLep() const;
  GenParticle WHad() const; 
  GenParticle ChargedLepton() const;
  GenParticle Neutrino() const;
  GenParticle Q1() const;
  GenParticle Q2() const; 

  
 private:

  GenParticle m_Top; 
  GenParticle m_Antitop;
  GenParticle m_WTop; 
  GenParticle m_WAntitop;
  GenParticle m_bTop; 
  GenParticle m_bAntitop;
  GenParticle m_Wdecay1; 
  GenParticle m_Wdecay2; 
  GenParticle m_WMinusdecay1; 
  GenParticle m_WMinusdecay2; 

  E_DecayChannel m_type;
};


class TTbarGenProducer: public uhh2::AnalysisModule {
public:
    explicit TTbarGenProducer(uhh2::Context & ctx, const std::string & name = "ttbargen", bool throw_on_failure = true);
    virtual bool process(uhh2::Event & event) override;
    
private:
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    bool throw_on_failure;
};


