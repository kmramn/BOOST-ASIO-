//
//  Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/locale.hpp>
#include <iostream>

#include <ctime>
// https://www.boost.org/doc/libs/1_48_0/libs/locale/doc/html/hello_8cpp-example.html

int main()
{
    //using namespace boost::locale;
    //using namespace std;
    boost::locale::generator gen;
    std::locale loc = gen("");
    // Create system default locale

    std::locale::global(loc);
    // Make it system global

    std::cout.imbue(loc);
    // Set as default locale for output

    std::cout << boost::locale::format("Today {1,date} at {1,time} we had run our first localization example") % time(0)
        << std::endl;

    std::cout << "This is how we show numbers in this locale " << boost::locale::as::number << 103.34 << std::endl;
    std::cout << "This is how we show currency in this locale " << boost::locale::as::currency << 103.34 << std::endl;
    std::cout << "This is typical date in the locale " << boost::locale::as::date << std::time(0) << std::endl;
    std::cout << "This is typical time in the locale " << boost::locale::as::time << std::time(0) << std::endl;
    std::cout << "This is upper case " << boost::locale::to_upper("Hello World!") << std::endl;
    std::cout << "This is lower case " << boost::locale::to_lower("Hello World!") << std::endl;
    std::cout << "This is title case " << boost::locale::to_title("Hello World!") << std::endl;
    std::cout << "This is fold case " << boost::locale::fold_case("Hello World!") << std::endl;

}

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4