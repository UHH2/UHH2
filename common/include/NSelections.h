#pragma once

#include "UHH2/core/include/Selection.h"

// This file defines multiplicity selections on jets, leptons, etc
// General conventions:
//  * Events are selected if the multiplicity is between >= nmin and <= nmax, i.e. nmin and nmax are allowed values
//  * nmax = -1 means to not apply a maximum cut (to not apply nmin cut, use nmin = 0).
//
// The modules usually apply the cut on the current number of objects; therefore,
// you might want to run a cleaning module before running a Selection module; see CleaningModules.h

class NMuonSelection: public uhh2::Selection {
public:
    explicit NMuonSelection(int nmin, int nmax = -1);
    virtual bool passes(const uhh2::Event & event);
private:
    int nmin, nmax;
};


class NElectronSelection: public uhh2::Selection {
public:
    explicit NElectronSelection(int nmin, int nmax = -1);
    virtual bool passes(const uhh2::Event & event);
private:
    int nmin, nmax;
};

class NJetSelection: public uhh2::Selection {
public:
    explicit NJetSelection(int nmin, int nmax = -1);
    virtual bool passes(const uhh2::Event & event);
private:
    int nmin, nmax;
};

