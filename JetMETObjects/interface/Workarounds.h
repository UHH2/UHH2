#ifndef CMSSW

#include <sstream>
#include <stdexcept>

// workarounds to make standalone compilation easier. -- Jochen Ott, Nov. 2014

using namespace std;

namespace edm {
    class LogError{
    public:
        explicit LogError(const std::string & s){
            msg << s << " ";
        }
        
        template<typename T>
        LogError & operator<<(T && t){
            msg << std::forward<T>(t);
            return *this;
        }
        
        ~LogError() noexcept(false) {
            throw std::runtime_error(msg.str());
        }
        
    private:
        std::stringstream msg;
    };
}

#else

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#endif
