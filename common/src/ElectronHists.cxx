#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/core/include/Event.h"

#include "TH1F.h"

using namespace uhh2;

ElectronHists::ElectronHists(Context & ctx, const std::string & dname): Hists(ctx, dname){
    number = book<TH1F>( "number","number of electrons",7,-0.5,6.5);
    pt = book<TH1F>( "pt","p_{T} electron",100,0,500);
    eta = book<TH1F>( "eta","#eta electron",100,-3,3);
    phi = book<TH1F>( "phi","#phi electron",100,-M_PI,M_PI);
    isolation = book<TH1F>("isolation","relIso electron",100,0,0.5);
}

void ElectronHists::fill(Event & event){
    auto w = event.weight;
    assert(event.electrons);

    number->Fill(event.electrons->size(), w);
    for(const auto & ele : *event.electrons){
        pt->Fill(ele.pt(), w);
        eta->Fill(ele.eta(), w);
        phi->Fill(ele.phi(), w);
        isolation->Fill(ele.relIso(), w);
    }
}



