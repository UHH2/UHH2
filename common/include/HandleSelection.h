#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Selection.h"
#include <string>
#include <stdexcept>


using namespace std;
using namespace uhh2;

/* \brief class HandleSelection, selection for Handles on basic types (int, float, ...)
 **/

template<typename HANDLETYPE>
class HandleSelection: public Selection {
public:
    explicit HandleSelection(Context & ctx,
                             const string & handlename,
                             HANDLETYPE min_val=-99999.0,
                             HANDLETYPE max_val=99999.0):
        name_(handlename),
        hndl(ctx.get_handle<HANDLETYPE>(handlename)),
        min_(min_val),
        max_(max_val) {}

    virtual bool passes(const Event & e) override {
        if (!e.is_valid(hndl)) {
            return false;
        }
        HANDLETYPE value = e.get(hndl);
        return min_ <= value && value <= max_;
    }

    const string &name() const {return name_;}

private:
    string name_;
    Event::Handle<HANDLETYPE> hndl;
    HANDLETYPE min_;
    HANDLETYPE max_;
};


