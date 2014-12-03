#pragma once

#include "UHH2/core/include/Event.h"

// see comments in ElectronIds.h for general comments on IDs.

class CSVBTag {
public:
    enum wp {WP_LOOSE, WP_MEDIUM, WP_TIGHT };
    
    explicit CSVBTag(wp working_point);
    
    bool operator()(const Jet & jet, const uhh2::Event & event) const;
private:
    float csv_threshold;
};

