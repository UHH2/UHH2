#include "UHH2/core/include/Utils.h"

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <limits>
#include <strings.h>

#include <boost/lexical_cast.hpp>

#include <cxxabi.h>

using namespace std;
using namespace uhh2;

bool uhh2::string2bool(const std::string & s){
    static const char * true_strings[] = {"true", "yes", "1", "on"};
    static const char * false_strings[] = {"false", "no", "0", "off"};
    for(size_t i=0; i<4; ++i){
        if(strcasecmp(s.c_str(), true_strings[i])==0) return true;
        if(strcasecmp(s.c_str(), false_strings[i])==0) return false;
    }
    throw std::runtime_error("could not interpret '" + s + "' as boolean");
}

std::string uhh2::double2string(double d){
    char buf[40];
    int res = snprintf(buf, 40, "%.18e", d);
    if(res <= 0 || res >= 40){
        throw runtime_error("double2string: snprintf returned unexpected result");
    }
    return buf;
}

std::string uhh2::double2string(double d, int precision){
    char buf[40], fbuf[10];
    int res = snprintf(fbuf, 10, "%%%d.5g", precision);
    if(res <= 0 || res >= 10){
        throw runtime_error("double2string: snprintf[0] returned unexpected result");
    }
    res = snprintf(buf, 40, fbuf, d);
    if(res <= 0 || res >= 40){
        throw runtime_error("double2string: snprintf[1] returned unexpected result");
    }
    return buf;
}

std::string uhh2::int2string(int i){
    char buf[40];
    int res = snprintf(buf, 40, "%d", i);
    if(res <= 0 || res >= 40){
        throw runtime_error("int2string: snprintf returned unexpected result");
    }
    return buf;
}

double uhh2::string2double(const std::string & s){
    return boost::lexical_cast<double>(s);
}

std::string uhh2::demangle(const std::string & mangled_typename){
    int status;
    char * demangled = abi::__cxa_demangle(mangled_typename.c_str(), 0, 0, &status);
    if(status != 0 or demangled == 0){
        free(demangled);
        return mangled_typename;
    }
    std::string result(demangled);
    free(demangled);
    return result;
}

void uhh2::trim(string & s, const std::string & to_trim){
    size_t p0 = s.find_first_not_of(to_trim);
    size_t p1 = s.find_last_not_of(to_trim);
    if(p0 != string::npos && p1 != string::npos){
        assert(p1 >= p0);
        s = s.substr(p0, p1 - p0 + 1);
    }
    else{
        s = "";
    }
}

std::vector<std::string> uhh2::split(const std::string & s, const std::string & splitting){
    size_t current_pos = 0;
    std::vector<std::string> result;
    while(true){
        size_t p0 = s.find_first_not_of(splitting, current_pos);
        if(p0 == string::npos) break;
        size_t p1 = s.find_first_of(splitting, p0);
        if(p1 == string::npos){
            // take up to end of string and we're done:
            result.emplace_back(s.substr(p0));
            break;
        }
        else{
            assert(p1 > p0);
            result.emplace_back(s.substr(p0, p1 - p0));
            current_pos = p1;
        }
    }
    return result;
}


TableOutput::TableOutput(const vector<std::string> & header_): ncols(header_.size()), header(header_){
}

namespace{
void hline(ostream & out, size_t total_width){
    out << " ";
    for(size_t i=1; i+1<total_width; ++i){
        out << "-";
    }
    out << endl;
}


void out_row(ostream & out, const vector<string> & row, const vector<size_t> colsize, const string & sep){
    const size_t ncols = colsize.size();
    assert(ncols == row.size());
    for(size_t i=0; i<ncols; ++i){
        out << sep << row[i];
        size_t nfill = colsize[i] - row[i].size();
        for(size_t j=0; j<nfill; ++j){
            out << " ";
        }
    }
    out << sep << endl;
}


}

void TableOutput::print(ostream & out){
    const string sep = " | ";
    // calculate column sizes, given by maximum entry:
    vector<size_t> colsize(ncols, 0);
    for(size_t i=0; i<ncols; ++i){
        colsize[i] = header[i].size();
    }
    for(vector<vector<string> >::const_iterator row = rows.begin(); row != rows.end(); ++row){
        for(size_t i=0; i<ncols; ++i){
            colsize[i] = max(colsize[i], (*row)[i].size());
        }
    }
    size_t total_width = 0;
    for(size_t i=0; i<ncols; ++i){
        total_width += sep.size() + colsize[i];
    }
    total_width += sep.size();
    
    // output table:
    hline(out, total_width);
    out_row(out, header, colsize, sep);
    hline(out, total_width);
    for(vector<vector<string> >::const_iterator row = rows.begin(); row != rows.end(); ++row){
        out_row(out, *row, colsize, sep);
    }
    hline(out, total_width);
}

// entries.size() == ncols must hold
void TableOutput::add_row(const vector<string> & row){
    assert(ncols == row.size());
    rows.push_back(row);
}


