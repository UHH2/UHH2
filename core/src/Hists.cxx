#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;
using namespace std;

Hists::Hists(Context & ctx_, const string & dirname_): ctx(ctx_), dirname(dirname_){
    trim(dirname, "/");
}

Hists::~Hists(){}

TH1* Hists::hist(const char * name){
    auto it = histos.find(name);
    if(it==histos.end()){
        throw std::runtime_error("Hists::get_hist: did not find histogram '" + string(name) + "'");
    }
    return it->second;
}
