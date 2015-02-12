#include "UHH2/common/include/GenTools.h"


GenParticle const * findMother(GenParticle const & igenp, vector<GenParticle> const * genparticles)
{
	GenParticle const * imother = igenp.mother(genparticles);
	if (!imother) imother = igenp.mother(genparticles, 2);
	return imother;
}


NGenParticleCalculator::NGenParticleCalculator(Context & ctx, std::string hndl_name, int genp_id, boost::optional<int> mother_id) :
		hndl_(ctx.get_handle<int>(hndl_name)), genp_id_(genp_id), mother_id_(mother_id) {}

bool NGenParticleCalculator::process(Event & event)
{
	int n_particles = 0;
	for (GenParticle const & genp : *event.genparticles)
	{
		if (abs(genp.pdgId()) == genp_id_)
		{
			if (mother_id_)
			{
				bool right_mother = false;
				GenParticle const * gen_mother = findMother(genp, event.genparticles);
				while (gen_mother)
				{
					if (abs(gen_mother->pdgId()) == *mother_id_)
					{
						right_mother = true;
						break;
					}
					gen_mother = findMother(*gen_mother, event.genparticles);
				}
				if (!right_mother)
					continue;
			}
			n_particles++;
		}
	}

	event.set(hndl_, n_particles);
	return true;
}