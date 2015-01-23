#pragma once

#include <map>


/** Storage for arbitrary additional int-indexed floats.
 * 
 * This class can be used as member of other physics classes
 * such as Jet, TopJet, Lepton to have the ability to store additional floats (such
 * as new b-tagging discriminators, new variables for top-tagging, new lepton ids)
 * while still keeping a backward-compatible data format. This allows an
 * easier transition when adding information to the dataformat, as old code
 * not relying on the new information can run unchanged, which is not the case
 * if adding new data members.
 */
class Tags {
public:
    
    // throes a runtime_error exception in case the tag is not available.
    float get_tag(int) const;
    
    bool has_tag(int) const;
    // set the value for some tag
    void set_tag(int, float);
    
    
    
private:
    //std::vector<std::pair<int, float>> tagdata;
    std::map<int, float> tagdata;
};

