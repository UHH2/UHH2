#pragma once

#include "UHH2/core/include/NtupleObjects.h"
#include "UHH2/core/include/GenericEvent.h"

namespace uhh2 {

/** \brief The event container.
 *
 * Based on the UHHanalysis' BaseCycleContainer. This class allows easy (direct) access to many frequently needed
 * objects, such as jets, electron, muons, etc. To store data beyond those directly declared data
 * members, use the GenericEvent methods, which allows settings and getting arbitrary data via handles.
 */
class Event: public GenericEvent {
public:
  double weight; // Note: unlike the other members, the weight is not read from the input file and not written to the output file, but calculated on the fly.

  int run;
  int luminosityBlock;
  long long event;
  std::string year;

  float rho;

  bool isRealData;

  bool passEcalBadCalib;

  float prefiringWeight;
  float prefiringWeightUp;
  float prefiringWeightDown;

  float beamspot_x0;
  float beamspot_y0;
  float beamspot_z0;
  std::vector< PrimaryVertex >* pvs;

  std::vector< Electron >* electrons;
  std::vector< Muon >* muons;
  std::vector< Tau >* taus;
  std::vector< Photon >* photons;
  std::vector< Jet >* jets;
  std::vector< TopJet >* topjets;
  std::vector< TopJet >* toppuppijets;
  MET* met;
  MET* genmet;
  std::vector< L1EGamma>* L1EG_seeds;
  std::vector< L1Jet>* L1J_seeds;
  std::vector< L1Muon>* L1M_seeds;
  std::vector< L1EtSum>* L1EtS_seeds;
  std::vector< L1Tau>* L1T_seeds;

  GenInfo* genInfo;
  std::vector< GenTopJet >* gentopjets;
  std::vector< GenParticle >* genparticles;
  std::vector< GenJet >* genjets;
  std::vector< PFParticle>* pfparticles;

  /** \brief Access to trigger results.
   *
   * Access to trigger results is treated differently from the other
   * event content in order to save disk space: In principle, the trigger
   * information saved here can be thought of as a map from string to bool,
   * where the key is the HLT trigger name and the value indicates whether or
   * not the event passed that trigger. However, the trigger names do not change
   * within a run, and saving the trigger names for each event would be a waste
   * of disk space. Therefore, the trigger names are only saved for the first
   * event for each run in that file, and only the vector of booleans of the actual
   * trigger results is saved for each event.
   *
   * Accessing the trigger decision is a two-step process: first get a \c TriggerIndex
   * with \c get_trigger_index. This one \c TriggerIndex can then be used for many events
   * with \c passes_trigger.
   * Internally, \c TriggerIndex stores the actual index into the vector of booleans of trigger
   * decisions for the runid last seen, thus avoiding to make string comparisons for each event.
   * If the runid changes, this index is looked up again in the (potentially changed) trigger
   * menu for the new run. This is done automatically in the routines below, the user does not have
   * to do anything special at a run transition.
   */
  struct TriggerIndex {
      // invalid triggerindex
      TriggerIndex(): index(-1), runid(0){}

  private:
      friend class Event;
      size_t index; // index into trigger results vector<bool>
      int runid; // runid for which index was computed
      std::string triggername; // name to look up the trigger again in case of runid change
      bool is_prefix;
      TriggerIndex(size_t i, int r, std::string tn, bool p): index(i), runid(r), triggername(std::move(tn)), is_prefix(p){}
  };

  /** \brief Get a new trigger index based on the name
   *
   * The argument \c triggername can contain a trailing wildcard, i.e., end with '*'
   * to match any characters. (Note that no other matching/wildcard is implemented).
   *
   * The trigger name used here should usually start with 'HLT_' and end with '_v*'.
   *
   * Throws an invalid_argument exception in case triggername is empty or "*". Note
   * that no trigger name lookup is performed at this point, so construction
   * will always succeed, even for triggers not available; use \c lookup_trigger_index
   * to test whether the trigger is actually available.
   */
  TriggerIndex get_trigger_index(const std::string & triggername) const;

  /** \brief Test whether the current event passes the trigger represented by the TriggerIndex
   *
   * Throws the same exceptions as \c lookup_trigger_index. In addition, throws a runtime_error
   * if \c lookup_trigger_index returns false.
   */
  bool passes_trigger(TriggerIndex & ti) const;

  /** \brief Returns the pre-scale factor of the trigger represented by the TriggerIndex
   *
   * Throws the same exceptions as \c lookup_trigger_index. In addition, throws a runtime_error
   * if \c lookup_trigger_index returns false.
   */
  int trigger_prescale(TriggerIndex & ti) const;

  //L1 prescales, see for more details:
  //https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2017#Trigger
  int trigger_prescaleL1min(TriggerIndex & ti) const;
  int trigger_prescaleL1max(TriggerIndex & ti) const;

  /** \brief Test whether a given trigger is available for the current event
   *
   * Note that this method is only needed to test whether a trigger is available at all;
   * it is not needed to test whether an event passes a trigger as \c passes_trigger
   * calls \c lookup_trigger_index automatically.
   *
   * Throws a runtime_error if the trigger name in \c ti matches multiple triggers.
   */
  bool lookup_trigger_index(TriggerIndex & ti) const;

  /** \brief Get the names of the triggers for the current event
   */
  const std::vector<std::string> & get_current_triggernames() const {
      return triggerNames_currentrun;
  }


  // ** Framework methods **
  // Analyses should not use the methods below directly; this is for adapting to the framework(s) only.
  using GenericEvent::RawHandle;
  template<class T> using Handle = GenericEvent::Handle<T>;

  std::vector<bool>* & get_triggerResults(){
      return triggerResults;
  }

  std::vector<int>* & get_triggerPrescales(){
      return triggerPrescales;
  }

  std::vector<int>* & get_triggerPrescalesL1min(){
    return triggerPrescalesL1min;
  }
  std::vector<int>* & get_triggerPrescalesL1max(){
    return triggerPrescalesL1max;
  }


  void set_triggernames(std::vector<std::string> names){ // for the current run(!)
      triggerNames_currentrun = move(names);
      triggerNames_currentrun_runid = run;
  }

  // clear all data.
  // Set all members pointer to zero, all floats to NAN, all integers to -1, and clear all vectors.
  void clear();


  Event(const GenericEventStructure & ges);

  ~Event();

private:
    std::vector<bool>* triggerResults;
    std::vector<int>* triggerPrescales;
    std::vector<int>* triggerPrescalesL1min;
    std::vector<int>* triggerPrescalesL1max;

    std::vector<std::string> triggerNames_currentrun;
    int triggerNames_currentrun_runid;
};

}
