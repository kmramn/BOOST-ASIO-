
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winerror.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket1;
char szBuffer1[100];
boost::asio::const_buffer buffer1(szBuffer1, 100);

void write_handler1(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	std::wcout << L"write_handler1" << std::endl;
	std::wcout << L"bytes_transferred:" << bytes_transferred << std::endl;
}

void connect_handler1(const boost::system::error_code& ec)
{
	std::cout << pSocket1->local_endpoint().address().to_string() << std::endl;
	std::cout << pSocket1->local_endpoint().port() << std::endl;
	std::cout << pSocket1->remote_endpoint().address().to_string() << std::endl;
	std::cout << pSocket1->remote_endpoint().port() << std::endl;

	//lstrcpyA(szBuffer1, "Hello Server.\n");
	lstrcpyA(szBuffer1, "The quick brown fox jumps over the lazy dog.\n");
	std::wcout << L"connect_handler1" << std::endl;
	std::cin.get();
	pSocket1->async_write_some(buffer1, boost::bind(write_handler1,
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	pSocket1->async_write_some(buffer1, boost::bind(write_handler1,
		boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void _tmain()
{
	std::wcout << L"Hello Client." << std::endl;
	boost::asio::io_context iocontext1;
	boost::asio::ip::tcp::endpoint endpoints1(boost::asio::ip::address::from_string("127.0.0.1"), 5800);
	//boost::asio::ip::tcp::endpoint endpoints1(boost::asio::ip::address::from_string("10.41.14.202"), 5800);
	//boost::asio::ip::tcp::socket socket1(iocontext1);
	pSocket1 = boost::make_shared<boost::asio::ip::tcp::socket>(iocontext1);
	std::cin.get();
	pSocket1->async_connect(endpoints1, boost::bind(connect_handler1, boost::asio::placeholders::error));
	iocontext1.run();
}

/*
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>
using namespace std;

#include <boost/asio.hpp>

#include <boost/bind.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

char data1[1024];
char data2[1024];
boost::shared_ptr<boost::asio::ip::tcp::socket> socketptr1;

bool bExit = true;
void my_write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	//lstrcpyA(data1, "Hello\n");
	wcout << "Bytes transferred : " << bytes_transferred << endl;
}

void my_read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	cout << data2 << endl;
	wcout << L"Byted read : " << bytes_transferred << endl;
	if (ec == boost::asio::error::eof)
		bExit = false;
}

void my_connect_handler(const boost::system::error_code& ec)
{
	lstrcpyA(data1, "Hello Server\n");
	socketptr1->async_write_some( boost::asio::const_buffer(data1, 13),
		boost::bind(my_write_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	do
	{
		socketptr1->async_read_some(boost::asio::mutable_buffer(data2, 3),
			boost::bind(my_read_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	} while (bExit);
	cout << "Exiting my_connect_handler" << endl;
}

void _tmain()
{
	boost::asio::io_context iocontext1;

	//boost::asio::ip::tcp::socket socket1(iocontext1);
	socketptr1 = boost::make_shared<boost::asio::ip::tcp::socket>(iocontext1);

	boost::asio::ip::tcp::endpoint endpoint1(
		boost::asio::ip::address(boost::asio::ip::address::from_string("127.0.0.1")), 18734);

	//socket1.async_connect(endpoint1, boost::bind(my_connect_handler, boost::asio::placeholders::error));
	socketptr1->async_connect(endpoint1, boost::bind(my_connect_handler, boost::asio::placeholders::error));

	iocontext1.run();
	cout << "Exiting Client" << endl;
}*/