#define BOOST_TEST_MODULE master

// save diagnostic state
#pragma GCC diagnostic push
// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <boost/test/included/unit_test.hpp>

// turn the warnings back on
#pragma GCC diagnostic pop
