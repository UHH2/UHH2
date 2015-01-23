#include "UHH2/core/include/Tags.h"
#include <sstream>

using namespace std;

float Tags::get_tag(int index) const {
    auto it = tagdata.find(index);
    if(it == tagdata.end()){
        stringstream ss;
        ss << "Did not find value for tag " << index;
        throw runtime_error(ss.str());
    }
    return it->second;
}

bool Tags::has_tag(int index) const{
    return tagdata.find(index) != tagdata.end();
}

void Tags::set_tag(int index, float value) {
    tagdata[index] = value;
}
