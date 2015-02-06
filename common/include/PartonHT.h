#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Selection.h"

class PartonHT: public uhh2::AnalysisModule {
public:
    
    explicit PartonHT(const uhh2::Event::Handle<double> & handle);
    
    virtual bool process(uhh2::Event & event) override;
    
    // calculate the parton ht value from the genparticles.
    // Provided as static function to also allow direct use instead of
    // the AnalysisModule / Handle mechanism.
    static double calculate(const std::vector<GenParticle> & genparticles);
    
private:
    uhh2::Event::Handle<double> handle;
};


/** select events which have a double value between certain values.
 * 
 * This can be used, for example, to select only those events from the inclusive W+Jets sample
 * with a certain partonht value.
 */
class DoubleSelection: public uhh2::Selection{
public:
    DoubleSelection(const uhh2::Event::Handle<double> & handle, double min_value, double max_value);
    virtual bool passes(const uhh2::Event & event) override;
    
private:
    uhh2::Event::Handle<double> handle;
    double min_value, max_value;
};

