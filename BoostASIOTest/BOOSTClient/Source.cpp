
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
    socket1.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 18793));
    std::string buff = "Hello from client\n";
    boost::asio::write(socket1, boost::asio::buffer(buff));
    boost::asio::write(socket1, boost::asio::buffer(buff));
}
