#include "UHH2/core/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/LuminosityHists.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

using namespace uhh2;
using namespace std;

    
bool operator<(const run_lumi & rl1, const run_lumi & rl2){
    if(rl1.run == rl2.run){
        return rl1.lumiblock < rl2.lumiblock;
    }
    else{
        return rl1.run < rl2.run;
    }
}


LuminosityHists::LuminosityHists(uhh2::Context & ctx,
                                const std::string & dirname,
				const std::string & triggername,
				bool do_inst_lumi_hist) :
    Hists(ctx, dirname),
    triggername_(triggername),
    do_inst_lumi_hist_(do_inst_lumi_hist)
{
    lumi_per_bin = string2double(ctx.get("lumihists_lumi_per_bin", "50.0"));
    if(lumi_per_bin <= 0.0) {
        throw runtime_error("lumihists_lumi_per_bin is <= 0.0; this is not allowed");
    }
    
    string lumifile = ctx.get("lumi_file");
    std::unique_ptr<TFile> file(TFile::Open(lumifile.c_str(), "read"));
    TTree * tree = dynamic_cast<TTree*>(file->Get("AnalysisTree"));
    if(!tree){
        throw runtime_error("LuminosityHists: Did not find TTree 'AnalysisTree' in file ;" + lumifile + "'");
    }
    // only fetch branches we really need:
    TBranch * brun = tree->GetBranch("run");
    TBranch * blumiblock = tree->GetBranch("luminosityBlock");
    TBranch * bilumi = tree->GetBranch("intgRecLumi");
    run_lumi rl;
    double ilumi;
    brun->SetAddress(&rl.run);
    blumiblock->SetAddress(&rl.lumiblock);
    bilumi->SetAddress(&ilumi);
    
    // use the file to build a map from run/lumi --> integrated lumi in pb.
    // Inserting into a map sorts by run and lumi.

    double total_lumi = 0.0; // in   1/pb
    double maxinstlumi = 0.0;
    auto ientries = tree->GetEntries();
    
    for(auto ientry = 0l; ientry < ientries; ientry++){
        for(auto b : {brun, blumiblock, bilumi}){
            b->GetEntry(ientry);
        }
        double ilumi_pb = ilumi * 1e-6; // convert units in file (microbarn) to pb.
	if(ilumi_pb > maxinstlumi) maxinstlumi = ilumi_pb;
        total_lumi += ilumi_pb;
        auto it_inserted = rl2lumi.insert(make_pair(rl, ilumi_pb));
        if(!it_inserted.second){
            throw runtime_error("Duplicate run/lumi entry in lumi file '" + lumifile + "'");
        }

    }
    //cout << "LuminosityHists: read total lumi " << total_lumi << " from lumi file " << lumifile << endl;
    
    // Save the bin borders to find out the number of bins to use and for later assigning each event to a bin.
    int nbins_estimated = int(total_lumi / lumi_per_bin + 1);
    if(nbins_estimated >= 20000){
        throw runtime_error("LuminosityHists misconfiguration: would take more than 20000 bins. Please increase lumi_per_bin");
    }
    upper_binborders.reserve(nbins_estimated);

    double ilumi_current_bin = 0.0;
    for(const auto & rl : rl2lumi){
       ilumi_current_bin += rl.second;
       if(ilumi_current_bin >= lumi_per_bin){
          upper_binborders.push_back(rl.first);
            ilumi_current_bin = ilumi_current_bin - lumi_per_bin;
        }
    }
    int nbins = upper_binborders.size() + 1; // add one for the partial bin
    hlumi = book<TH1D>("luminosity", "Events over time divided in equal lumi-size bins", nbins, 0, ( int(total_lumi / lumi_per_bin) + 1)*lumi_per_bin);
    //hlumi = book<TH1D>("luminosity", "Events over time divided in equal lumi-size bins", nbins, 0, nbins);

    if(do_inst_lumi_hist_){
      hinstlumi = book<TH1D>("luminosity_per_lumi_section", "#Events/LS/pb", nbins, 0, maxinstlumi);
      hinstlumi_ref = new TH1D("luminosity_per_lumi_section_ref", "Number of lumi sections", nbins, 0, maxinstlumi);
      
      //fill the reference histogram with number of lumisections for each inst. lumi bin
      for(auto ientry = 0l; ientry < ientries; ientry++){
	for(auto b : {brun, blumiblock, bilumi}){
	  b->GetEntry(ientry);
	}
	double ilumi_pb = ilumi * 1e-6;
	hinstlumi_ref->Fill(ilumi_pb);
      }
    }
    
}
    
void LuminosityHists::fill(const uhh2::Event & ev){
    if(!ev.isRealData) return;
    

    bool trigger_accepted = true;
    if (!triggername_.empty()) {
        auto trg_index = ev.get_trigger_index(triggername_);
        trigger_accepted = ev.passes_trigger(trg_index);
    }

    if (trigger_accepted) {
        run_lumi rl{ev.run, ev.luminosityBlock};
        auto it = upper_bound(upper_binborders.begin(), upper_binborders.end(), rl);
        int ibin = distance(upper_binborders.begin(), it); // can be upper_bounds.size() at most, which is nbins and thus Ok.
        hlumi->Fill(ibin*lumi_per_bin, ev.weight); // weight is usually 1.0 anyway, but who knows ...


	if(do_inst_lumi_hist_){
	  double ilumi=0;
	  
	  for(const auto & rll : rl2lumi){
	    if( rl.run==rll.first.run && rl.lumiblock==rll.first.lumiblock){
	      ilumi = rll.second;
	      break;
	    }
	  }
	  
	  int i = hinstlumi->FindBin(ilumi);
	  if(hinstlumi_ref->GetBinContent(i)!=0){
	    hinstlumi->SetBinContent(i, hinstlumi->GetBinContent(i)+ev.weight/hinstlumi_ref->GetBinContent(i)/hinstlumi_ref->GetBinCenter(i));
	    hinstlumi->SetBinError(i,  sqrt( hinstlumi->GetBinError(i)*hinstlumi->GetBinError(i) + (ev.weight/hinstlumi_ref->GetBinContent(i)/hinstlumi_ref->GetBinCenter(i)) * (ev.weight/hinstlumi_ref->GetBinContent(i)/hinstlumi_ref->GetBinCenter(i)) ));
	  }
	}
    }

}

