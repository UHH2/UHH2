#include "UHH2/core/include/obj.h"

using namespace uhh2;

void obj::fail_type(const std::type_info & ti) const {
    throw std::invalid_argument("obj: type mismatch");
}

obj::obj(const std::type_info & ti, void * ptr, std::function<void (void*)> eraser_): type_(ti), addr(ptr), eraser(std::move(eraser_)){
}

