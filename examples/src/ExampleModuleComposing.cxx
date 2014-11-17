#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

using namespace std;
using namespace uhh2;

/* Example for composing many AnalysisModules
 * 
 * This example shows how to use AnalysisModules together by having one 'main' 
 * AnalysisModule which calls one or more other modules. It also shows several ways
 * of how to pass information between different modules.
 * ExampleModuleComposing will be the main (top-level) AnalysisModule called by the
 * AnalysisModuleRunner. In addition, four more AnalysisModules are implemented:
 * 
 *  - CalcHT1, which calculates HT and stores the result in the event, using GenericEvent capabilities.
 *    This is an example of how to 'produce' data.
 *  - PrintHT1 prints the HT value using the GenericEvent methods to print the value calculated by CalcHT1
 *  - CalcHT2 also calculates HT (actually -HT), but uses references of member variables of ExampleModuleComposing instead.
 *  - PrintHT2 prints the value of HT calculated by CalcHT2, using the reference mechnism
 * 
 * In general, the examples will be more complicated, but the main idea will remain the same:
 * one module 'produces' data (CalcHT) and one or more other modules 'consume' that data, and
 * one has to pass the produced information between those places somehow.
 * 
 * The strength of this 'composing' approach lies in the fact that the 'consuming' modules do not need
 * to know which module produced the data. This abstraction allows to easily test different algorithms
 * without re-writing all the code.
 */

class CalcHT1: public AnalysisModule {
public:
    explicit CalcHT1(Context & ctx){
        ht_handle = ctx.get_handle<double>("HT");
    }
    
    virtual bool process(Event & event){
        double ht = 0.0;
        // note: in this example, we just use jets. In general, HT is defined
        // in quite different ways ...
        for(const auto & jet : *event.jets){
            ht += jet.pt();
        }
        event.set(ht_handle, ht);
        return true;
    }
private:
    Event::Handle<double> ht_handle;
};

class PrintHT1: public AnalysisModule {
public:
    explicit PrintHT1(Context & ctx){
        ht_handle = ctx.get_handle<double>("HT");
    }
    
    virtual bool process(Event & event){
        cout << "PrintHT1: ht = " << event.get(ht_handle) << endl;
        return true;
    }
private:
    Event::Handle<double> ht_handle;
    
};

class CalcHT2: public AnalysisModule {
public:
    explicit CalcHT2(double & htresult): ht(htresult){
    }
    
    virtual bool process(Event & event){
        ht = 0.0;
        for(const auto & jet : *event.jets){
            ht -= jet.pt();
        }
        return true;
    }
private:
    double & ht; // note the reference, no copy!
};

class PrintHT2: public AnalysisModule {
public:
    explicit PrintHT2(const double & ht_): ht(ht_){}
    
    virtual bool process(Event & event){
        cout << "PrintHT2: ht = " << ht << endl;
        return true;
    }
private:
    const double & ht;
};



class ExampleModuleComposing: public AnalysisModule {
public:
    
    explicit ExampleModuleComposing(Context & ctx);
    virtual bool process(Event & event);

private:
    // instead of a vector<unique_ptr<AnalysisModule>>, one could also use
    // members CalcHT1 and CalcHT2 directly. However, using the vector is easier
    // to initialize and allows to add some modules easily.
    std::vector<std::unique_ptr<AnalysisModule>> modules;
    
    // data member to pass information:
    double ht;
};

ExampleModuleComposing::ExampleModuleComposing(Context & ctx) {
    // construct all modules, make sure to create CalcHT before PrintHT, as
    // modules will be called in the order of construction.
    modules.emplace_back(new CalcHT1(ctx));
    modules.emplace_back(new PrintHT1(ctx));
    modules.emplace_back(new CalcHT2(ht));
    modules.emplace_back(new PrintHT2(ht));
}


bool ExampleModuleComposing::process(Event & event) {
    // just call all modules' process method:
    for(const auto & m : modules){
        m->process(event);
    }
    return true;
}

// we only register ExampleModuleComposing here, as the other modules
// are not required. Note that we could register CalcHT1 without
// any problems because it has the right constructor, while this
// would not be possible for CalcHT2, as it does not have the right constructor.
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleComposing)
