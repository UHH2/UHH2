#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/Selection.h"

namespace uhh2 {

/// Select events with certain minimum / maximum number of jets
class NJetSelection: public Selection {
public:
    /// In case nmax=-1, no cut on the maximum is applied.
    explicit NJetSelection(int nmin, int nmax = -1);
    virtual bool passes(const Event & event) override;
    
private:
    int nmin, nmax;
};


/** \brief Various definitions of b-tagging, in particular working points
 * 
 * This is useful for various selection modules, and thus defined outside of a particular Selection class.
 */
namespace btagging {
    
enum class csv_wp {
    loose, medium, tight
};

/// convert a CSV working point to a numerical threshold of the discriminator.
float csv_threshold(const csv_wp & wp);

}

/// Select events with certain minimum / maximum number of b-tagged jets using the CSV tagger
class NBTagSelection: public Selection {
public:
    /// In case nmax=-1, no cut on the maximum is applied.
    explicit NBTagSelection(int nmin, int nmax = -1, btagging::csv_wp wp = btagging::csv_wp::medium);
    virtual bool passes(const Event & event) override;
    
private:
    int nmin, nmax;
    float min_csv;
};

}
