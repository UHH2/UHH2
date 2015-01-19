#pragma once

#include "UHH2/core/include/Event.h"

// see also ElectronIds.h for general comments


class TauIDMedium {
public:
    bool operator()(const Tau & tau, const uhh2::Event & event) const;
};


