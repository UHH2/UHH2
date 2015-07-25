#include "UHH2/common/include/JetIds.h"

using namespace std;
using namespace uhh2;

CSVBTag::CSVBTag(wp working_point) {
    switch(working_point){
        case WP_LOOSE:
            csv_threshold = 0.605f;
            break;
        case WP_MEDIUM:
            csv_threshold = 0.890f;
            break;
        case WP_TIGHT:
            csv_threshold = 0.970f;
            break;
        default:
            throw invalid_argument("invalid working point passed to CSVBTag");
    }
}


bool CSVBTag::operator()(const Jet & jet, const Event &) const{
    return jet.btag_combinedSecondaryVertex() > csv_threshold;
}
