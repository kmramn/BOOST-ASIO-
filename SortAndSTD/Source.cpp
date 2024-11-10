/*
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <boost/sort/spreadsort/spreadsort.hpp>

using namespace std;
using namespace boost::sort::spreadsort;

#define DATA_TYPE int

//[rightshift_int_functor
struct rightshift {
    inline int operator()(DATA_TYPE x, unsigned offset) { return x >> offset; }
};
//] [/rightshift_int_functor]

class TestC
{
public:
    TestC(){}
};

void main()
{
    TestC();
    vector<int> v1;

    v1.push_back(345);
    v1.push_back(67);
    v1.push_back(456);
    v1.push_back(1);
    v1.push_back(89);
    v1.push_back(789);
    v1.push_back(42);
    v1.push_back(56);
    printf("Before Sort:\n");
    for (int i = 0; i < v1.size(); i++)
    {
        printf("%d\n", v1[i]);
    }
    //rightshift();
    //sort(v1.begin(), v1.end());
    //spreadsort(v1.begin(), v1.end());
    integer_sort(v1.begin(), v1.end(), rightshift());
    printf("After Sort:\n");
    for (int i = 0; i < v1.size(); i++)
    {
        printf("%d\n", v1[i]);
    }
}
*/

//! \brief Integer sort with a rightshift functor sorting example.
//! \file
//
//  Copyright Steven Ross 2009-2014.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/sort for library home page.

// Caution: this file contains Quickbook markup as well as code
// and comments, don't change any of the special comment markups!

#include <winerror.h>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace boost::sort::spreadsort;
using namespace boost::random;

#define DATA_TYPE int

//[rightshift_int_functor
struct rightshift {
    inline int operator()(DATA_TYPE x, unsigned offset)
    {
        return x >> offset;
    }
};
//] [/rightshift_int_functor]

//Pass in an argument to test std::sort
int main(int argc, const char ** argv)
{
    size_t uCount, uSize = sizeof(DATA_TYPE);
    bool stdSort = false;
    unsigned loopCount = 1;
    for (int u = 1; u < argc; ++u)
    {
        if (std::string(argv[u]) == "-std")
            stdSort = true;
        else
            loopCount = atoi(argv[u]);
    }

    std::ifstream input("input.txt", std::ios_base::in | std::ios_base::binary);
    if (input.fail())
    {
        printf("input.txt could not be opened\n");
        return 1;
    }

    double total = 0.0;
    std::vector<DATA_TYPE> array;
    input.seekg(0, std::ios_base::end);
    size_t length = input.tellg();
    uCount = length / uSize;
    //Run multiple loops, if requested
    for (unsigned u = 0; u < loopCount; ++u)
    {
        input.seekg(0, std::ios_base::beg);
        //Conversion to a vector
        array.resize(1100);
        unsigned v = 0;
        mt19937 gen;
        for (int i = 0; i < 1100; i++)
        {
            uniform_int_distribution<> dist(1, 100);
            array[v++] = dist(gen);
        }
        //while (input.good() && v < uCount)
        //    input.read(reinterpret_cast<char *>(&(array[v++])), uSize);
        if (v < uCount)
            array.resize(v);
        clock_t start, end;
        double elapsed;
        start = clock();
        if (stdSort)
        {
            std::sort(array.begin(), array.end());
        }
        else
        {
            //[rightshift_1
            integer_sort(array.begin(), array.end(), rightshift());
            //] [/rightshift_1]
        }
        end = clock();
        elapsed = static_cast<double>(end - start);
        std::ofstream ofile;
        if (stdSort)
            ofile.open("standard_sort_out.txt", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        else
            ofile.open("boost_sort_out.txt", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
        if (ofile.good())
        {
            for (unsigned v = 0; v < array.size(); ++v)
            {
                ofile.write(reinterpret_cast<char *>(&(array[v])), sizeof(array[v]));
            }
            ofile.close();
        }
        total += elapsed;
        array.clear();
    }

    if (stdSort)
        printf("std::sort elapsed time %f\n", total / CLOCKS_PER_SEC);
    else
        printf("spreadsort elapsed time %f\n", total / CLOCKS_PER_SEC);

    return 0;
}
