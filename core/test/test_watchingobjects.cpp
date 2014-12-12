#include <boost/test/unit_test.hpp>

#include "UHH2/core/include/WatchingObjects.h"

using namespace uhh2;

BOOST_AUTO_TEST_SUITE(watchingobjects)

BOOST_AUTO_TEST_CASE(create_set) {
    WatchingObjects wo;
    WatchingObjects::Handle h_test;
    BOOST_CHECK_THROW(wo.get_value<int>(h_test), std::invalid_argument);
    
    h_test = wo.put("test", obj::create<int>(0));
    //BOOST_CHECK_EQUAL(wo.name(h_test), "test");
    
    wo.assign<int>(h_test, 5);
    BOOST_CHECK_EQUAL(wo.get_value<int>(h_test), 5);
}


BOOST_AUTO_TEST_CASE(watch) {
    WatchingObjects wo;
    WatchingObjects::Handle h_test = wo.put("test", obj::create<int>(0));
    
    int cb_value = 0;
    std::function<void (const int &)> callback = [&cb_value](const int & i){cb_value = i;};
    wo.watch_after<int>("test", callback);
    
    wo.assign<int>(h_test, 5);
    BOOST_CHECK_EQUAL(wo.get_value<int>(h_test), 5);
    BOOST_CHECK_EQUAL(cb_value, 5);
    
    wo.assign<int>(h_test, 42);
    BOOST_CHECK_EQUAL(wo.get_value<int>(h_test), 42);
    BOOST_CHECK_EQUAL(cb_value, 42);
}

BOOST_AUTO_TEST_CASE(watch_wrongtype) {
    WatchingObjects wo;
    wo.put("test", obj::create<float>(0));
    
    int cb_value = 0;
    std::function<void (const int &)> callback = [&cb_value](const int & i){cb_value = i;};
    BOOST_CHECK_THROW(wo.watch_after<int>("test", callback), std::logic_error);
}

BOOST_AUTO_TEST_CASE(watch2) {
    WatchingObjects wo;
    WatchingObjects::Handle h_test = wo.put("test", obj::create<int>(0));
    
    int cb_value = 0;
    std::function<void (const int &)> callback = [&cb_value](const int & i){cb_value = i;};
    wo.watch_after<int>("test", callback);
    
    int cb_value2 = 0;
    std::function<void (const int &)> callback2 = [&cb_value2](const int & i){cb_value2 = i;};
    wo.watch_after<int>("test", callback2);
    
    wo.assign<int>(h_test, 5);
    BOOST_CHECK_EQUAL(wo.get_value<int>(h_test), 5);
    BOOST_CHECK_EQUAL(cb_value, 5);
    BOOST_CHECK_EQUAL(cb_value2, 5);
}

BOOST_AUTO_TEST_CASE(watch_before_create) {
    WatchingObjects wo;
    
    int cb_value = 0;
    std::function<void (const int &)> callback = [&cb_value](const int & i){cb_value = i;};
    wo.watch_after<int>("test", callback);
    
    WatchingObjects::Handle h_test = wo.put("test", obj::create<int>(0));
    
    wo.assign<int>(h_test, 5);
    BOOST_CHECK_EQUAL(cb_value, 5);
}

BOOST_AUTO_TEST_CASE(watch_before_create_wrongtype) {
    WatchingObjects wo;
    
    int cb_value = 0;
    std::function<void (const int &)> callback = [&cb_value](const int & i){cb_value = i;};
    wo.watch_after<int>("test", callback);
    
    BOOST_CHECK_THROW(wo.put("test", obj::create<float>(0)), std::logic_error);
}

BOOST_AUTO_TEST_CASE(set_wrongtype){
    WatchingObjects wo;
    WatchingObjects::Handle h_test = wo.put("test", obj::create<int>(0));
    
    BOOST_CHECK_THROW(wo.assign<float>(h_test, 5.0f), std::logic_error);
}

BOOST_AUTO_TEST_CASE(invalid_handle){
    WatchingObjects wo;
    WatchingObjects::Handle h_test;
    
    BOOST_CHECK_THROW(wo.assign<float>(h_test, 5.0f), std::logic_error);
}

BOOST_AUTO_TEST_CASE(double_create){
    WatchingObjects wo;
    wo.put("test", obj::create<int>(0));
    
    BOOST_CHECK_THROW(wo.put("test", obj::create<int>(0)), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
