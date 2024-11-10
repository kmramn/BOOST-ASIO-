
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

boost::mutex g_mutex;

void WorkerThread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "WorkerThread1: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();

	while (true)
	{
		boost::system::error_code ec;
		try
		{
			pIOContext->run(ec);
			if (ec)
			{
				g_mutex.lock();
				std::cout << "Error: " << ec << std::endl;
				g_mutex.unlock();
			}
			break;
		}
		catch (std::runtime_error& ex)
		{
			g_mutex.lock();
			std::cout << "Exception: " << ex.what() << std::endl;
			g_mutex.unlock();
		}
	}
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));

	boost::shared_ptr<boost::asio::io_context::strand>
		pStrand1(new boost::asio::io_context::strand(*pIOContext1));

	boost::thread_group tg1;

	tg1.create_thread(boost::bind(WorkerThread1, pIOContext1));
	tg1.create_thread(boost::bind(WorkerThread1, pIOContext1));

	boost::asio::ip::tcp::socket socket1(*pIOContext1);
	try
	{

		boost::asio::ip::tcp::resolver resolver1(*pIOContext1);
		//boost::asio::ip::tcp::resolver::query query1("www.google.com", "80");
		boost::asio::ip::tcp::resolver::iterator itr1 = resolver1.resolve("www.google.com", "80");
		boost::asio::ip::tcp::endpoint ep1 = *itr1;

		socket1.connect(ep1);
	}
	catch (std::exception& ex)
	{
		g_mutex.lock();
		std::cout << "Network Exception!" << ex.what() << std::endl;
		g_mutex.unlock();
	}

	std::cin.get();

	boost::system::error_code ec;
	socket1.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	socket1.close();

	pIOContext1->stop();

	tg1.join_all();
}