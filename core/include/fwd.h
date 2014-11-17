// This file is to forward-declare classes in uhh2. It is useful to avoid cyclic header dependencies.
// Usually, this file should be included first, and the actual class headers should only be included when really needed.
// (This also helps saving compile time, as fewer includes are needed).
namespace uhh2 {
    // AnalysisModule.h:
    class AnalysisModule;
    class Context;
    
    // Hists.h
    class Hists;
    
    // Event.h
    class Event;
    
    // GenericEvent.h
    class GenericEventStructure;
    class GenericEvent;
}


// also forward-declare some root classes:
class TH1;
class TH1F;
class TH1D;


#ifdef __CINT__
#define override
#endif
