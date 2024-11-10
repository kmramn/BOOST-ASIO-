//#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

//#include "..\ExampleLib\Header.h"

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    //Func();
    using namespace boost::lambda;
    typedef std::istream_iterator<int> in;

    /*std::for_each(
        in(std::cin), in(), std::cout << (_1 * 3) << " ");*/
    boost::shared_ptr< boost::asio::io_service > io_service(new boost::asio::io_service);

}
