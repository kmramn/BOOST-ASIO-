
#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

#include <Windows.h>

void __stdcall Func()
{
    MessageBoxW(NULL, L"Test", L"Test", MB_OK);
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " ");

}
