#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/Event.h"
#include <string>
#include <map>
#include <set>
#include <vector>

namespace uhh2 {

namespace detail {

/** \brief This is a framework-internal class; do not use it.
 *
 * This helper class aids the framework to fill the contents of the Event container
 * correctly: The framework for event input and output is tightly coupled to the GenericEvent,
 * so all data read is read into members of GenericEvent, and the output written is the
 * one in GenericEvent. However, for easier access, the Event class has data members
 * to access the most widely-used event data directly (e.g. Event::run, Event::event, Event::jets, etc.).
 * This helper class provides methods to synchronize these data.
 *
 * This helper class contains routines to
 *  - fill the Event class data members from the GenericEvent content; this would be called just
 *    after reading the event from the input file (as the input is read into the GenericEvent members)
 *  - copy the values from the Event class to the GenericEvent; this should be called just before writing the event output;
 *    the output written is the content of the GenericEvent, so to allow modules to change the Event content
 *    by accessing the Event members, this change has to be synchronized back to the GenericEvent.
 *
 * The class also deals with the special case of triggerNames:
 *   - when reading events, the framework has to lookup all runs and fill set_triggernames
 *   - this helper class will make sure the right trigger names are set in the event
 *   - when writing, the triggerNames are only non-empty if the event is the first one for the current run
 *
 * It is assumed that
 *  - one EventHelper exists per Event; so a separate EventHelper is required for each Event
 *  - only one output file exists per EventHelper (this is important for deciding whether or not to write the trigger).
 */

class EventHelper {
public:
    // note: the EventHelper will always set up input AND output.
    // If not output is desired, this has to be dealt within the Context object.
    explicit EventHelper(uhh2::Context & ctx);

    // should be called right after construction, before reading the first file.
    // Set the branch names for the direct members of Event. Note
    // that for 'plain' members (run, event, lumi, isRealData, beamspot etc.) the names
    // are hard-coded and are not set explicitly.
    //
    // If bname is empty, the call has no effect.
    void setup_pvs(const std::string & bname);
    void setup_electrons(const std::string & bname);
    void setup_muons(const std::string & bname);
    void setup_taus(const std::string & bname);
    void setup_photons(const std::string & bname);
    void setup_jets(const std::string & bname);
    void setup_topjets(const std::string & bname);
    void setup_toppuppijets(const std::string & bname);
    void setup_met(const std::string & bname);
    void setup_genmet(const std::string & bname);

    void setup_genInfo(const std::string & bname);
    void setup_gentopjets(const std::string & bname);
    void setup_genparticles(const std::string & bname);
    void setup_pfparticles(const std::string & bname);
    void setup_genjets(const std::string & bname);

    void setup_L1EG_seeds(const std::string & bname);
    void setup_L1J_seeds(const std::string & bname);
    void setup_L1M_seeds(const std::string & bname);

    void setup_trigger();

    // note: event has to outlive this class. If creating a new event, also create a new EventHelper!
    void set_event(Event * event);

    // this is called at the beginning of each input file
    void set_infile_triggernames(std::map<int, std::vector<std::string>> triggernames);

    // this is called just after reading an event; it copies the GenericEvent content to the Event
    void event_read();

    // this is called just before writing an event; it copies the Event data members to the GenericEvent ones
    void event_write();

private:

    uhh2::Context & ctx;

    Event * event;

    bool pvs, electrons, muons, taus, photons, jets, topjets, toppuppijets, met, genmet;
    bool genInfo, gentopjets, genparticles, genjets;
    bool pfparticles;
    bool trigger;
    bool L1EG_seeds, L1J_seeds, L1M_seeds;
    bool first_event_read;

    // trigger handling:
    std::map<int, std::vector<std::string>> triggernames;
    std::set<int> triggernames_written;
    int triggernames_last_runid_event;

    // handles:
    Event::Handle<int> h_run, h_lumi, h_event;
    Event::Handle<float> h_rho, h_bsx, h_bsy, h_bsz, h_prefire, h_prefireUp, h_prefireDown;
    Event::Handle<bool> h_isRealData, h_passEcalBadCalib;
    Event::Handle<std::string> h_year;

    Event::Handle<std::vector<PrimaryVertex>> h_pvs;
    Event::Handle<std::vector<Electron>> h_electrons;
    Event::Handle<std::vector<Muon>> h_muons;
    Event::Handle<std::vector<Tau>> h_taus;
    Event::Handle<std::vector<Photon>> h_photons;
    Event::Handle<std::vector<Jet>> h_jets;
    Event::Handle<std::vector<TopJet>> h_topjets;
    Event::Handle<std::vector<TopJet>> h_toppuppijets;
    Event::Handle<MET> h_met;
    Event::Handle<MET> h_genmet;

    Event::Handle<GenInfo> h_genInfo;
    Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    Event::Handle<std::vector<GenParticle>> h_genparticles;
    Event::Handle<std::vector<PFParticle>> h_pfparticles;
    Event::Handle<std::vector<GenJet>> h_genjets;

    Event::Handle<std::vector<bool>> h_triggerResults;
    Event::Handle<std::vector<int>> h_triggerPrescales;
    Event::Handle<std::vector<int>> h_triggerPrescalesL1min;
    Event::Handle<std::vector<int>> h_triggerPrescalesL1max;
    Event::Handle<std::vector<std::string>> h_triggerNames;
    Event::Handle<std::vector<L1EGamma>> h_L1EG_seeds;
    Event::Handle<std::vector<L1Jet>> h_L1J_seeds;
    Event::Handle<std::vector<L1Muon>> h_L1M_seeds;

};


}

}
