#include "UHH2/common/include/TTbarReconstruction.h"
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include <cassert>

using namespace uhh2;
using namespace std;

PrimaryLepton::PrimaryLepton(Context & ctx) {
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

bool PrimaryLepton::process(uhh2::Event & event) {
    assert(event.muons || event.electrons);
    double ptmax = -infinity;
    FlavorParticle primlep;
    if(event.electrons) {
        for(const auto & ele : *event.electrons) {
            if(ele.pt() > ptmax) {
                ptmax = ele.pt();
                primlep = ele;
            }
        }
    }
    if(event.muons) {
        for(const auto & mu : *event.muons) {
            if(mu.pt() > ptmax) {
                ptmax = mu.pt();
                primlep = mu;
            }
        }
    }
    event.set(h_primlep, std::move(primlep));
    return true;
}

PrimaryLepton::~PrimaryLepton() {}

HighMassTTbarReconstruction::HighMassTTbarReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const string & label): m_neutrinofunction(neutrinofunction) {
    h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

HighMassTTbarReconstruction::~HighMassTTbarReconstruction() {}

bool HighMassTTbarReconstruction::process(uhh2::Event & event) {
    assert(event.jets);
    assert(event.met);
    //find primary charged lepton
    const Particle & lepton = event.get(h_primlep);
    std::vector<ReconstructionHypothesis> recoHyps;
    //reconstruct neutrino
    std::vector<LorentzVector> neutrinos = m_neutrinofunction( lepton.v4(), event.met->v4());
    unsigned int n_jets = event.jets->size();
    if(n_jets>10) n_jets=10; //avoid crashes in events with many jets
    // idea: loop over 3^Njet possibilities and write the current loop
    // index j in the 3-base system. The Njets digits represent whether
    // to assign each jet to the hadronic side (0), leptonic side (1),
    // or none of them (2).
    const unsigned int max_j = pow(3, n_jets);

    //loop over neutrino solutions and jet assignments to fill hyotheses
    for(const auto & neutrino_p4 : neutrinos) {
        const LorentzVector wlep_v4 = lepton.v4() + neutrino_p4;
        for (unsigned int j=0; j < max_j; j++) {
            LorentzVector tophad_v4;
            LorentzVector toplep_v4 = wlep_v4;
            int hadjets=0;
            int lepjets=0;
            int num = j;
            ReconstructionHypothesis hyp;
            hyp.set_lepton(lepton);
            hyp.set_neutrino_v4(neutrino_p4);
            for (unsigned int k=0; k<n_jets; k++) {
                if(num%3==0) {
                    tophad_v4 = tophad_v4 + event.jets->at(k).v4();
                    hyp.add_tophad_jet(event.jets->at(k));
                    hadjets++;
                }

                if(num%3==1) {
                    toplep_v4 = toplep_v4 + event.jets->at(k).v4();
                    hyp.add_toplep_jet(event.jets->at(k));
                    lepjets++;
                }
                //in case num%3==2 do not take this jet at all
                //shift the trigits of num to the right:
                num /= 3;
            }
            //fill only hypotheses with at least one jet assigned to each top quark
            if(hadjets>0 && lepjets>0) {
                hyp.set_tophad_v4(tophad_v4);
                hyp.set_toplep_v4(toplep_v4);
                recoHyps.emplace_back(move(hyp));
            }
        } // 3^n_jets jet combinations
    } // neutrinos
    event.set(h_recohyps, move(recoHyps));
    return true;
}


TopTagReconstruction::TopTagReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const string & label): m_neutrinofunction(neutrinofunction) {
    h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

bool TopTagReconstruction::process(uhh2::Event & event) {
    std::cerr<< "TopTagger is not yet implemented in UHH2! As soon as it is implemented you can import this function from TopFitCalc::CalculateTopTag in UHHAnalysis." << std::endl;
    return true;
}

TopTagReconstruction::~TopTagReconstruction() {}

std::vector<LorentzVector> NeutrinoReconstruction(const LorentzVector & lepton, const LorentzVector & met) {
    TVector3 lepton_pT = toVector(lepton);
    lepton_pT.SetZ(0);
    TVector3 neutrino_pT = toVector(met);
    neutrino_pT.SetZ(0);
    const float mass_w = 80.399;
    float mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;
    float A = - (lepton_pT * lepton_pT);
    float B = mu * lepton.pz();
    float C = mu * mu - lepton.e() * lepton.e() * (neutrino_pT * neutrino_pT);
    float discriminant = B * B - A * C;
    std::vector<LorentzVector> solutions;
    if (0 >= discriminant) {
        // Take only real part of the solution
        //
        LorentzVectorXYZE solution (0,0,0,0);
        solution.SetPx(met.Px());
        solution.SetPy(met.Py());
        solution.SetPz(-B / A);
        solution.SetE(toVector(solution).Mag());

        solutions.emplace_back(toPtEtaPhi(solution));
    }
    else {
        discriminant = sqrt(discriminant);
        LorentzVectorXYZE solution (0,0,0,0);
        solution.SetPx(met.Px());
        solution.SetPy(met.Py());
        solution.SetPz((-B - discriminant) / A);
        solution.SetE(toVector(solution).Mag());

        solutions.push_back(toPtEtaPhi(solution));

        LorentzVectorXYZE solution2 (0,0,0,0);
        solution2.SetPx(met.Px());
        solution2.SetPy(met.Py());
        solution2.SetPz((-B + discriminant) / A);
        solution2.SetE(toVector(solution2).Mag());

        solutions.push_back(toPtEtaPhi(solution2));
    }

    return solutions;
}

double DeltaPolarNeutrino(double PhiN, double metPx, double metPy, double PhiLep, double PtLep) {
    double PyN;
    double PxN;
    const double mass_w = 80.399;

    double num = 10.e-7;

    if(1-cos(fabs(PhiLep-PhiN)> M_PI ? 2*M_PI-fabs(PhiLep-PhiN) : fabs(PhiLep-PhiN )) < num) {
        PyN = 0.5*mass_w*mass_w* sin(PhiN)/(PtLep*num);
        PxN = 0.5*mass_w*mass_w* cos(PhiN)/(PtLep*num);
    }
    else {
        PyN = 0.5*mass_w*mass_w* sin(PhiN)/(PtLep*(1-cos(fabs(PhiLep-PhiN)> M_PI ? 2*M_PI-fabs(PhiLep-PhiN) : fabs(PhiLep-PhiN ))));
        PxN = 0.5*mass_w*mass_w* cos(PhiN)/(PtLep*(1-cos(fabs(PhiLep-PhiN)> M_PI ? 2*M_PI-fabs(PhiLep-PhiN) : fabs(PhiLep-PhiN ))));
    }
    return pow(PxN-metPx,2)+pow(PyN-metPy,2);
}

static void polarminuitfunc(int& nDim, double* gout, double& result, double par[], int flg) {
    result = DeltaPolarNeutrino(par[0],par[1],par[2],par[3],par[4]);
}

std::vector<LorentzVector> NeutrinoFitPolar(const LorentzVector & lepton, const LorentzVector & met) {
    TVector3 lepton_pT = toVector(lepton);
    lepton_pT.SetZ(0);
    TVector3 neutrino_pT = toVector(met);
    neutrino_pT.SetZ(0);
    const double mass_w = 80.399;
    double start = met.phi();
    double step = 10.e-5;
    double mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;
    double A = - (lepton_pT * lepton_pT);
    double B = mu * lepton.pz();
    double C = mu * mu - lepton.e() * lepton.e() * (neutrino_pT * neutrino_pT);
    double discriminant = B * B - A * C;
    std::vector<LorentzVector> solutions;
    if (0 >= discriminant) {

        double resultPhi = 0;
        double error = 0;
        int ierflg;
        std::unique_ptr<double[]> arg(new double[1]);

        std::unique_ptr<TMinuit> positiv(new TMinuit(5));
        positiv->SetPrintLevel(-1); // -1 quiet, 0 normal, 1 verbose; Preset 0

        positiv->SetFCN(polarminuitfunc);

        positiv->DefineParameter(0,"PhiN",start, step,  -2*M_PI, 2*M_PI);
        positiv->DefineParameter(1,"metPx",met.px(),0,0,0);
        positiv->DefineParameter(2,"metPy",met.py(),0,0,0);
        positiv->DefineParameter(3,"PhiLep",lepton.phi(),0,0,0);
        positiv->DefineParameter(4,"PtLep",lepton.pt(),0,0,0);

        positiv->FixParameter(1);
        positiv->FixParameter(2);
        positiv->FixParameter(3);
        positiv->FixParameter(4);

        positiv->SetMaxIterations(500);
        arg[0]= 2;
        positiv->mnexcm("SET STR",arg.get(),1,ierflg);
        positiv->Migrad();
        positiv->GetParameter(0,resultPhi,error);
        if(resultPhi != resultPhi) {
            std::cerr << "neutrino phi is NAN " << std::endl;
        }
        if(resultPhi > M_PI) resultPhi = resultPhi-2*M_PI;
        if(resultPhi < M_PI) resultPhi = resultPhi+2*M_PI;

        double PyN;
        double PxN;

        double num = 10.e-7;

        if(1-cos(deltaPhiAbs(lepton.phi(), resultPhi)) < num) {
            PyN = 0.5*mass_w*mass_w* sin(resultPhi)/(lepton.pt()*num);
            PxN = 0.5*mass_w*mass_w* cos(resultPhi)/(lepton.pt()*num);
        }
        else {
            PyN = 0.5*mass_w*mass_w* sin(resultPhi)/(lepton.pt()*(1-cos(deltaPhiAbs(lepton.phi(), resultPhi))));
            PxN = 0.5*mass_w*mass_w* cos(resultPhi)/(lepton.pt()*(1-cos(deltaPhiAbs(lepton.phi(), resultPhi))));
        }

        LorentzVectorXYZE neutrino_result(0,0,0,0);
        neutrino_result.SetPx(PxN);
        neutrino_result.SetPy(PyN);

        double pzfit =  lepton.pz()*neutrino_result.pt()/lepton.pt();

        LorentzVectorXYZE solution (0,0,0,0);
        solution.SetPx(PxN);
        solution.SetPy(PyN);
        solution.SetPz(pzfit);
        solution.SetE(toVector(solution).Mag());


        solutions.push_back(toPtEtaPhi(solution));

    } else {
        discriminant = sqrt(discriminant);

        LorentzVectorXYZE solution (0,0,0,0);
        solution.SetPx(met.Px());
        solution.SetPy(met.Py());
        solution.SetPz((-B - discriminant) / A);
        solution.SetE(toVector(solution).Mag());

        solutions.push_back(toPtEtaPhi(solution));

        LorentzVectorXYZE solution2 (0,0,0,0);
        solution2.SetPx(met.Px());
        solution2.SetPy(met.Py());
        solution2.SetPz((-B + discriminant) / A);
        solution2.SetE(toVector(solution2).Mag());

        solutions.push_back(toPtEtaPhi(solution2));

    }

    return solutions;
}

