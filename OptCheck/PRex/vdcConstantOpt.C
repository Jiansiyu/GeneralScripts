/*
 * Used for calculate the focal plane variables from the detector plane variabls
 * Author: Siyu Jian
 *      sj9va@virginia.edu
 *
 */
#include "vector"
#include "assert.h"


enum { kPORDER = 7 };

class THaMatrixElement {
public:
    THaMatrixElement() : iszero(true), order(0), v(0)
    { pw.reserve(5); poly.reserve(kPORDER); }
    bool match( const THaMatrixElement& rhs ) const
    { assert(pw.size() == rhs.pw.size()); return ( pw == rhs.pw ); }
    void clear()
    { iszero = true; pw.clear(); order = 0; v = 0.0; poly.clear(); }

    bool iszero;             // whether the element is zero
    std::vector<int> pw;     // exponents of matrix element
    //   e.g. D100 = { 1, 0, 0 }
    int  order;
    double v;                // its computed value
    std::vector<double> poly;// the associated polynomial
};


