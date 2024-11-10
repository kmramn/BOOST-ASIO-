
/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

char szBuffer1[100];
//boost::asio::mutable_buffer buffer1(szBuffer1, 100);
boost::asio::mutable_buffer buffer1(szBuffer1, 10);

void read_handler1(const boost::system::error_code& ec, std::size_t bytes_transferred,
	boost::shared_ptr<boost::asio::ip::tcp::socket>& pSocket)
{
	std::wcout << L"read_handler1" << std::endl;
	std::wcout << L"bytes transferred: " << bytes_transferred << std::endl;
	std::cout << szBuffer1 << std::endl;
	if (!ec)
	{
		//pSocket->async_read_some(buffer1,
		//	boost::bind(read_handler1, boost::asio::placeholders::error,
		//		boost::asio::placeholders::bytes_transferred, pSocket));
		pSocket->async_read_some(buffer1,
			boost::bind(read_handler1, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, pSocket));
	}
	else
	{
		std::cout << "Error: " << ec << std::endl;
	}
}

void accept_handler1(const boost::system::error_code& ec, 
	boost::shared_ptr<boost::asio::io_context>& pIOContext,
	boost::shared_ptr<boost::asio::ip::tcp::socket>& pSocket,
	boost::shared_ptr<boost::asio::ip::tcp::acceptor>& paccept)
{
	std::wcout << L"accept_handler1" << std::endl;
	pSocket->async_read_some(buffer1,
		boost::bind(read_handler1, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, pSocket));

	boost::shared_ptr<boost::asio::ip::tcp::socket>
		pSocket1(new boost::asio::ip::tcp::socket(*pIOContext));
	paccept->async_accept(*pSocket1,
		boost::bind(accept_handler1, boost::asio::placeholders::error, pIOContext, pSocket1, paccept));
}

void _tmain()
{
	std::wcout << L"Hello Server." << std::endl;

	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::ip::tcp::endpoint>
		endpoint1(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 20000));
	boost::shared_ptr<boost::asio::ip::tcp::acceptor>
		paccept1(new boost::asio::ip::tcp::acceptor(*pIOContext1, *endpoint1));
	//boost::asio::ip::tcp::socket socket1(iocontext1);
	boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket1;
	pSocket1 = boost::make_shared<boost::asio::ip::tcp::socket>(*pIOContext1);

	paccept1->async_accept(*pSocket1,
		boost::bind(accept_handler1, boost::asio::placeholders::error, pIOContext1, pSocket1, paccept1));
	pIOContext1->run();
}*/


/*
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <string>

using namespace std;

#include <boost/asio.hpp>

#include <boost/bind.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/make_shared.hpp>

char data1[1024];
char data2[1024];
boost::shared_ptr<boost::asio::ip::tcp::socket> clientsoketptr;
bool bExit = true;
void my_read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	cout << data1 << endl;
	wcout << L"Byted read : " << bytes_transferred << endl;
	if (ec == boost::asio::error::eof)
		bExit = false;
}

void my_write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	lstrcpyA(data1, "Hello\n");
	wcout << "Bytes transferred : " << bytes_transferred << endl;
}

void my_accept_handler(const boost::system::error_code& ec)
{
	do
	{
		clientsoketptr->async_read_some(boost::asio::mutable_buffer(data1, 3),
			boost::bind(my_read_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

	} while (bExit);
	lstrcpyA(data2, "Hello Client\n");
	clientsoketptr->async_write_some(boost::asio::const_buffer(data2, 13),
		boost::bind(my_write_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	cout << "Exiting my_accept_handler" << endl;
}

void _tmain()
{
	boost::asio::io_context iocontext1;

	//boost::asio::ip::tcp::acceptor acceptor1(iocontext1,
	//	boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 18734));
	boost::asio::ip::tcp::acceptor acceptor1(iocontext1,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(), 18734));

	//boost::asio::ip::tcp::socket clientsoket1(iocontext1);
	clientsoketptr = boost::make_shared<boost::asio::ip::tcp::socket>(iocontext1);
	acceptor1.async_accept(*clientsoketptr, boost::bind(my_accept_handler, boost::asio::placeholders::error));
	iocontext1.run();
	cout << "Exiting Server" << endl;
	//Sleep(10000);
}*/

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS

char szBuffer1[100];
//boost::asio::mutable_buffer buffer1(szBuffer1, 100);
boost::asio::mutable_buffer buffer1(szBuffer1, 10);

void read_handler1(const boost::system::error_code& ec, std::size_t bytes_transferred,
	boost::shared_ptr<boost::asio::ip::tcp::socket>& pSocket)
{
	std::wcout << L"read_handler1" << std::endl;
	std::wcout << L"bytes transferred: " << bytes_transferred << std::endl;
	std::cout << szBuffer1 << std::endl;
	if (!ec)
	{
		//pSocket->async_read_some(buffer1,
		//	boost::bind(read_handler1, boost::asio::placeholders::error,
		//		boost::asio::placeholders::bytes_transferred, pSocket));
		pSocket->async_read_some(buffer1,
			boost::bind(read_handler1, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, pSocket));
	}
	else
	{
		std::cout << "Error: " << ec << std::endl;
	}
}

void accept_handler1(const boost::system::error_code& ec,
	boost::shared_ptr<boost::asio::io_context>& pIOContext,
	boost::shared_ptr<boost::asio::ip::tcp::socket>& pSocket,
	boost::shared_ptr<boost::asio::ip::tcp::acceptor>& paccept)
{
	std::wcout << L"accept_handler1" << std::endl;
	boost::shared_ptr<boost::asio::ip::tcp::socket>
		pSocket1(new boost::asio::ip::tcp::socket(*pIOContext));
	paccept->async_accept(*pSocket1,
		boost::bind(accept_handler1, boost::asio::placeholders::error, pIOContext, pSocket1, paccept));

	pSocket->async_read_some(buffer1,
		boost::bind(read_handler1, boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred, pSocket));
}

void _tmain()
{
	std::wcout << L"Hello Server." << std::endl;

	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());
	boost::thread_group tg;

	boost::shared_ptr<boost::asio::ip::tcp::endpoint>
		endpoint1(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5800));
	boost::shared_ptr<boost::asio::ip::tcp::acceptor>
		paccept1(new boost::asio::ip::tcp::acceptor(*pIOContext1, *endpoint1));
	//boost::asio::ip::tcp::socket socket1(iocontext1);
	boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket1;
	pSocket1 = boost::make_shared<boost::asio::ip::tcp::socket>(*pIOContext1);

	paccept1->async_accept(*pSocket1,
		boost::bind(accept_handler1, boost::asio::placeholders::error, pIOContext1, pSocket1, paccept1));

	tg.create_thread(boost::bind(&boost::asio::io_context::run, pIOContext1));

	std::cin.get();

	pIOContext1->stop();

	tg.join_all();
}
