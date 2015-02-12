#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

using namespace uhh2;
using namespace std;

/* Find the mother of a GenParticle; in case the first mother is not defined, it also checks the second mother of the particle
 * (not sure if this is a likely scenario). Return null vector if GenParticle has no mothers (note: in the hypothetical case that
 * a particle has two mothers, this function will return the second mother).
 */

GenParticle const * findMother(GenParticle const & p, vector<GenParticle> const * genp);




/* Count the number of GenParticles with a given pdgId in the event i.e. count the number of occurences of a specific type of particle.
 * Bear in mind that this module always checks for the absolute value of the pdgId so it cannot differentiate between particle and anti-particle.
 * 
 * If one only wants to count particles that originate from the decay of another particle (e.g. count only b's from top decays) one can 
 * provide the pdgId of that particle as an optional argument in the constructor of this module after the pdgId of the particle one
 * actually wants to count. Note that this module checks the entire history of the counted particle e.g. one can also count
 * muons from top decays by leaving out the intermediate W boson and directly provide the top id as second argument along with the muon id.
 */

class NGenParticleCalculator : public AnalysisModule {
public:
    explicit NGenParticleCalculator(Context & ctx, std::string hndl_name, int genp_id, boost::optional<int> mother_id = boost::none);

    virtual bool process(Event & event);

private:
    Event::Handle<int> hndl_;
    int genp_id_;
    boost::optional<int> mother_id_;
};