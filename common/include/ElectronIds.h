#pragma once

#include "UHH2/core/include/Event.h"

// Note that the electronIds are implemented not by inheriting from a common base class, but
// rather as classes that can be used as functions with the signature
//   bool electron_id(const Electron &, const Event &);
// This allows to write selections or other modules taking these ids as
//  std::function<bool (const Electron &, const Event &)>
// and allows to pass a function pointer directly if testing a new id.
// Note that the id function takes the Event as argument; this might look unnecessary at
// first, but actually quite some ids require accessing other event information, e.g. lepton
// ids require pileup density event.rho to calculate corrected PF isolation, or the jet-lepton
// DeltaR and ptrel.


class ElectronID_TrigMVA53 {
public:
    
    ElectronID_TrigMVA53(double ptmin, double etamax, double maxreliso);
    bool operator()(const Electron & electron, const uhh2::Event & event) const;
    
private:
    double ptmin, etamax, maxreliso;
};
