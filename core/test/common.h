#pragma once

// calculate some unique 'event data' for later
// checking of the result. Here: calculate some luminosityBlock
// from run and event number:
inline int calc_lumi(int run, int event){
    return run * 10000 + event;
}

inline bool calc_trig1(int run, int event){
    return (run + event) % 2 == 0;
}


inline bool calc_trig2(int run, int event){
    return (run + event) % 3 == 0;
}
