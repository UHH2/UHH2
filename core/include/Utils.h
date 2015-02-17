#pragma once

/* This file contains various utility methods which can be used in various places. 
 * If you think about adding a method here, keep in mind that this file is really only meant for  
 * very generic functions, such that could even be found in a very general (non-SFrame) library.
 * If this is not the case for the method you intend to add, this file is probably not the right place.
 */

#include <string>
#include <vector>
#include <memory>
#include <limits>

#define DEPRECATED(reason) __attribute__((deprecated(reason)))


namespace uhh2{

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args){
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/** \brief Interpret the string as a boolean
 * 
 * Accepts "1"/"0"  "on"/"off"  "true"/"false"  "yes"/"no", ignoring case.
 * If the string is anything but those values, a runtime_error is thrown.
 */
bool string2bool(const std::string & s);

/// Convert a double to a string; use enough digits to keep high accuracy
std::string double2string(double d);

/// Convert a double to string with a given precision
std::string double2string(double d, int precision);

/// Convert an integer to a string
std::string int2string(int i) DEPRECATED("use std::to_string instead");

/// Convert a string to a double
double string2double(const std::string & s);

/// Make a C++-mangled typename human-readable
std::string demangle(const std::string & mangled_typename);

/** \brief Remove leading and trailing characters (e.g. spaces) from a string
 * 
 * The default is to remove spaces, newlines, carriage returns and tabs.
 */
void trim(std::string & s, const std::string & to_trim = " \r\n\t");


/** Split a string at any of the given splitting characters (default: spacee-characters " \r\n\t").
 * 
 * It makes no difference for the result whether elements are separated by a single or multiple
 * consecutive splitting-characters.
 * Returns an empty vector if (and only if) s consists entirely of splitting-characters.
 */
std::vector<std::string> split(const std::string & s, const std::string & splitting = " \r\n\t");

/// for those who do not like std::numeric_limits<double>::infinity();
const constexpr double infinity = std::numeric_limits<double>::infinity();

/** \brief Format information in a text-based table
 * 
 * Useful for printing a table to stdout or to a file. After constructing, call 'add_row' for each row. Finally,
 * use the \c print method to print the formatted table. Note that the table can be printed to any stream, e.g.
 * cout, but also (via ofstream) to a file.
 */
class TableOutput{
public:
    
    /// construct, setting the column headings. Note that this defines (now and for always) the number of columns of this table.
    explicit TableOutput(const std::vector<std::string> & header);
    
    /// Append a row to the end of the table; the number of columns must match the one defined at the time of construction
    void add_row(const std::vector<std::string> & row);
    
    /// print the whole table to the supplied stream.
    void print(std::ostream & out);
    
private:
    size_t ncols;
    std::vector<std::string> header;
    std::vector<std::vector<std::string> > rows;
};

}
