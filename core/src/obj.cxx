#include "UHH2/core/include/obj.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;

void obj::fail_type(const std::type_info & ti) const {
    throw std::invalid_argument("obj type mismatch: obj stores data of type '" + demangle(type_.name()) + "', but tried to access with type '" + demangle(ti.name()) + "'");
}

obj::obj(const std::type_info & ti, void * ptr, std::function<void (void*)> eraser_): type_(ti), addr(ptr), eraser(std::move(eraser_)){
}

