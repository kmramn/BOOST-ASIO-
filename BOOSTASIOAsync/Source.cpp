
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <iostream>

boost::shared_ptr<boost::asio::io_service> io_service1;
boost::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor1;
char szData[1024];

void handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    std::cout << "BOOST AsyncClient" << std::endl;
    std::cout << szData << std::endl;
}

void AcceptFunc(boost::shared_ptr<boost::asio::ip::tcp::socket> clientSocket1)
{
    clientSocket1->async_read_some(boost::asio::buffer(szData, 1024),
        boost::bind(handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    //boost::shared_ptr<boost::asio::ip::tcp::socket> clientsocket1(
    //    new boost::asio::ip::tcp::socket(acceptor1->get_executor().context()));
    //acceptor1->async_accept(*clientsocket1, boost::bind(AcceptFunc, clientsocket1));
}

void main()
{
    io_service1 = boost::make_shared<boost::asio::io_service>();
    acceptor1 = boost::make_shared<boost::asio::ip::tcp::acceptor>(
        *io_service1, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 18793));
    boost::shared_ptr<boost::asio::ip::tcp::socket> clientsocket1(
        new boost::asio::ip::tcp::socket(*io_service1));
    acceptor1->async_accept(*clientsocket1, boost::bind(AcceptFunc, clientsocket1));
    io_service1->run();
}
                   