#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <boost/asio.hpp>
#include <iostream>

void main()
{
    boost::asio::io_service io_service1;
    boost::asio::ip::tcp::socket socket1(io_service1);
    boost::asio::ip::tcp::acceptor acceptor1(io_service1,
    boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 18793));
    /*boost::asio::ip::tcp::acceptor acceptor1(io_service1, 
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 18793));*/
    acceptor1.accept(socket1);
    boost::asio::streambuf buff;
    boost::asio::read_until(socket1, buff, "\n");
    wprintf(L"%S\n", boost::asio::buffer_cast<const WCHAR*>(buff.data()));
}