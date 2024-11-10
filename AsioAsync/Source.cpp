// https://www.gamedev.net/blog/950/entry-2249317-a-guide-to-getting-started-with-boostasio/?pg=4

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>
using namespace std;

#include <boost/asio.hpp>

#include <boost/shared_ptr.hpp>


void _tmain()
{
	boost::asio::io_context iocontext1;
	//boost::asio::io_service::work work1(iocontext1);
	shared_ptr<boost::asio::io_context::work> workptr1(new boost::asio::io_context::work(iocontext1));
	workptr1.reset();
	iocontext1.run();

	//for (int i = 0; i < 40; i++)
	//{
	//	cout << "poll : " << iocontext1.poll() << endl;
	//	cout << "i : " << i << endl;
	//}

	cout << "End of main()" << endl;
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/thread.hpp>

void worker_thread(shared_ptr<boost::asio::io_context> iocontext1)
{
	wcout << L"worker_thread start" << endl;
	iocontext1->run();
	wcout << L"worker_thread end" << endl;
}

void _tmain()
{
	//boost::asio::io_context iocontext1;
	shared_ptr<boost::asio::io_context> iocontextptr1(new boost::asio::io_context());
	boost::asio::io_service::work work1(*iocontextptr1);
	//shared_ptr<boost::asio::io_context::work> workptr1(new boost::asio::io_context::work(iocontext1));

	boost::thread_group worker_threads;

	for (int i = 0; i < 4; i++)
	{
		worker_threads.create_thread(boost::bind(&worker_thread, iocontextptr1));
	}

	cin.get();

	//iocontext1.stop();
	iocontextptr1->stop();

	worker_threads.join_all();

	cout << "End of main()" << endl;
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

boost::mutex g_mutex1;
void thread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex1.lock();
	std::cout << boost::this_thread::get_id() << std::endl;
	g_mutex1.unlock();

	pIOContext->run();
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> piOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::io_context::work> 
		pWork1(new boost::asio::io_context::work(*piOContext1));

	boost::thread_group tg;

	tg.create_thread(boost::bind(thread1, piOContext1));
	
	std::cin.get();

	piOContext1->stop();

	tg.join_all();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/timer.hpp>

boost::mutex g_mutex1;

void workerthread(boost::shared_ptr<boost::asio::io_context>& ioContext)
{
	g_mutex1.lock();
	std::cout << "workerthread: " << boost::this_thread::get_id() << std::endl;
	g_mutex1.unlock();

	ioContext->run();

	g_mutex1.lock();
	std::cout << "workerthread stopped: " << boost::this_thread::get_id() << std::endl;
	g_mutex1.unlock();
}

int Fibbonacy(int n)
{
	if (n <= 1)
		return n;

	return (Fibbonacy(n - 1) + Fibbonacy(n - 2));
}

void PrintFibbonacy(int n)
{
	std::cout << "PrintFibbonacy: " << boost::this_thread::get_id() << std::endl;
	for (int i = 0; i < n; i++)
	{
		g_mutex1.lock();
		std::cout << "Thread: " << boost::this_thread::get_id() << " " << Fibbonacy(i) << std::endl;
		g_mutex1.unlock();
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}
	std::cout << "PrintFibbonacy End: " << boost::this_thread::get_id() << std::endl;
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));
	
	boost::thread_group tg;

	tg.create_thread(boost::bind(workerthread, pIOContext1));
	tg.create_thread(boost::bind(workerthread, pIOContext1));

	std::cout << "Stop1" << std::endl;
	pIOContext1->stop();
	//pIOContext1->reset();

	std::cout << "Post1" << std::endl;
	pIOContext1->post(boost::bind(PrintFibbonacy, 5));

	std::cin.get();

	std::cout << "Post2" << std::endl;
	pIOContext1->post(boost::bind(PrintFibbonacy, 10));

	std::cin.get();

	std::cout << "Stop2" << std::endl;
	pIOContext1->stop();
	//pIOContext1->reset();

	std::cout << "join_all" << std::endl;
	tg.join_all();
	std::cout << "join_all end" << std::endl;
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

boost::mutex g_mutex;

void workerthread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "workerthread1: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();

	pIOContext->run();

	g_mutex.lock();
	std::cout << "workerthread1 end: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
}

void Dispatch(int x)
{
	g_mutex.lock();
	std::cout << "Dispatch: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
	g_mutex.lock();
	std::cout << "x: " << x << std::endl;
	g_mutex.unlock();
}

void Post(int x)
{
	g_mutex.lock();
	std::cout << "Post: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
	g_mutex.lock();
	std::cout << "x: " << x << std::endl;
	g_mutex.unlock();
}

void Run1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "Run1: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
	for (int i = 0; i < 3; i++)
	{
		g_mutex.lock();
		std::cout << "Dispatch: " << i << std::endl;
		g_mutex.unlock();
		pIOContext->dispatch(boost::bind(Dispatch, i * 2));
		g_mutex.lock();
		std::cout << "Post: " << i << std::endl;
		g_mutex.unlock();
		pIOContext->post(boost::bind(Post, i * 2 + 1));
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}
	g_mutex.lock();
	std::cout << "Run1 completed: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext(new boost::asio::io_context);
	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext));

	boost::thread_group tg;

	tg.create_thread(boost::bind(workerthread1, pIOContext));

	pIOContext->post(boost::bind(Run1, pIOContext));

	pIOContext->reset();

	tg.join_all();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

boost::mutex g_mutex;

void workerthread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "workerthread1: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
	pIOContext->run();
}

void printnumber(int n)
{
	std::cout << "printnumber: " << boost::this_thread::get_id() << " n: " << n << std::endl;
}

void main()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));

	//boost::shared_ptr<boost::asio::io_context::strand>
	//	pStrand1(new boost::asio::io_context::strand(*pIOContext1));

	boost::asio::io_context::strand Strand1(*pIOContext1);

	boost::thread_group tg;

	tg.create_thread(boost::bind(workerthread1, pIOContext1));
	tg.create_thread(boost::bind(workerthread1, pIOContext1));

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	Strand1.post(boost::bind(printnumber, 5));
	Strand1.post(boost::bind(printnumber, 3));
	Strand1.post(boost::bind(printnumber, 4));
	Strand1.post(boost::bind(printnumber, 2));
	Strand1.post(boost::bind(printnumber, 1));

	//pIOContext1->post(boost::bind(printnumber, 1));
	//pIOContext1->post(boost::bind(printnumber, 2));
	//pIOContext1->post(boost::bind(printnumber, 3));
	//pIOContext1->post(boost::bind(printnumber, 4));
	//pIOContext1->post(boost::bind(printnumber, 5));

	//pIOContext1->stop();
	pWork1.reset();
	tg.join_all();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

boost::mutex g_mutex;

void workerthread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "workerthread1: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
	pIOContext->run();
}

void printnumber(int n)
{
	std::cout << "printnumber: " << boost::this_thread::get_id() << " n: " << n << std::endl;
	//boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
}

void main()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));

	//boost::shared_ptr<boost::asio::io_context::strand>
	//	pStrand1(new boost::asio::io_context::strand(*pIOContext1));

	boost::asio::io_context::strand Strand1(*pIOContext1);

	boost::thread_group tg;

	tg.create_thread(boost::bind(workerthread1, pIOContext1));
	tg.create_thread(boost::bind(workerthread1, pIOContext1));

	boost::this_thread::sleep(boost::posix_time::milliseconds(100));

	//pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 1)));
	//std::cout << "After post" << std::endl;

	pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 1)));
	pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 2)));
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 3)));
	pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 4)));
	boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 5)));
	pIOContext1->post(Strand1.wrap(boost::bind(printnumber, 6)));

	//pIOContext1->stop();
	pWork1.reset();
	tg.join_all();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

boost::mutex g_mutex;

void worker_thread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	try
	{
		pIOContext->run();
	}
	catch (std::exception& ex)
	{

	}
}

void worker_thread2(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	boost::system::error_code er;

	while (true)
	{
		try
		{
			pIOContext->run(er);
			if (er)
			{
				g_mutex.lock();
				std::cout << "Error: " << er << std::endl;
				g_mutex.unlock();
				break;
			}
		}
		catch (std::runtime_error& ex)
		{
		}
	}
}

void ThrowException(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "ThrowException: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();

	pIOContext->post(boost::bind(ThrowException, pIOContext));
	throw(std::runtime_error("Error!"));
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());
	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));

	boost::thread_group tg;

	tg.create_thread(boost::bind(worker_thread2, pIOContext1));
	tg.create_thread(boost::bind(worker_thread2, pIOContext1));

	pIOContext1->post(boost::bind(ThrowException, pIOContext1));

	pIOContext1->stop();

	tg.join_all();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>

boost::mutex g_mutex;

void worker_thread2(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_mutex.lock();
	std::cout << "worker_thread2: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();

	boost::system::error_code er;

	while (true)
	{
		try
		{
			pIOContext->run(er);
			if (er)
			{
				g_mutex.lock();
				std::cout << "Error: " << er << std::endl;
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

	g_mutex.lock();
	std::cout << "worker_thread2 end: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();
}

void TimerHandler(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer>& dlTimer)
{
	g_mutex.lock();
	std::cout << "TimerHandler: " << boost::this_thread::get_id() << std::endl;
	g_mutex.unlock();

	if (ec)
	{
		g_mutex.lock();
		std::cout << "Error: " << ec << std::endl;
		g_mutex.unlock();
	}
	else
	{
		g_mutex.lock();
		std::cout << "Time out " << std::endl;
		g_mutex.unlock();
		dlTimer->expires_from_now(boost::posix_time::seconds(5));
		dlTimer->async_wait(boost::bind(TimerHandler, boost::asio::placeholders::error, dlTimer));
	}
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());
	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));

	boost::thread_group tg;

	tg.create_thread(boost::bind(worker_thread2, pIOContext1));
	tg.create_thread(boost::bind(worker_thread2, pIOContext1));

	boost::shared_ptr<boost::asio::deadline_timer> pdltimer(new boost::asio::deadline_timer(*pIOContext1));
	pdltimer->expires_from_now(boost::posix_time::seconds(5));
	pdltimer->async_wait(boost::bind(TimerHandler, boost::asio::placeholders::error, pdltimer));

	g_mutex.lock();
	std::cout << "get" << std::endl;
	g_mutex.unlock();
	std::cin.get();

	g_mutex.lock();
	std::cout << "Stop:" << std::endl;
	g_mutex.unlock();
	pIOContext1->stop();

	g_mutex.lock();
	std::cout << "join_all:" << std::endl;
	g_mutex.unlock();
	tg.join_all();

	g_mutex.lock();
	std::cout << "Finished" << std::endl;
	g_mutex.unlock();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>

boost::mutex g_mutex;

void worker_thread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	std::cout << "[" << boost::this_thread::get_id() << "] nworker_thread1" << std::endl;
	pIOContext->run();
}

void PrintNum(int n)
{
	std::cout << "[" << boost::this_thread::get_id() << "] n: " << n << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(1));
}

void TimerHandler(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer>& pdlTimer)
{
	//std::cout << "[" << boost::this_thread::get_id() << "] TimerHandler" << std::endl;
	if (ec)
	{
		g_mutex.lock();
		std::cout << "Error: " << ec << std::endl;
		g_mutex.unlock();
	}
	else
	{
		std::cout << "TimerHandler: " << boost::this_thread::get_id() << std::endl;
		pdlTimer->expires_from_now(boost::posix_time::seconds(1));
		pdlTimer->async_wait(boost::bind(TimerHandler, boost::asio::placeholders::error, pdlTimer));
	}
}

void _tmain()
{
	std::cout << "_tmain(): " << boost::this_thread::get_id() << std::endl;

	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());

	boost::shared_ptr<boost::asio::io_context::work>
		pWork1(new boost::asio::io_context::work(*pIOContext1));

	boost::shared_ptr<boost::asio::io_context::strand> pStrand(new boost::asio::io_context::strand(*pIOContext1));

	boost::shared_ptr<boost::asio::deadline_timer> pdlTimer(new boost::asio::deadline_timer(*pIOContext1));

	boost::thread_group tg;

	tg.create_thread(boost::bind(worker_thread1, pIOContext1));
	tg.create_thread(boost::bind(worker_thread1, pIOContext1));

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	pStrand->post(boost::bind(PrintNum, 1));
	pStrand->post(boost::bind(PrintNum, 2));
	pStrand->post(boost::bind(PrintNum, 3));

	pStrand->dispatch(boost::bind(PrintNum, 33));
	pIOContext1->dispatch(boost::bind(PrintNum, 33));

	pStrand->post(boost::bind(PrintNum, 4));
	pStrand->post(boost::bind(PrintNum, 5));
	pStrand->post(boost::bind(PrintNum, 6));

	pStrand->post(boost::bind(PrintNum, 1));
	pStrand->post(boost::bind(PrintNum, 2));
	pStrand->post(boost::bind(PrintNum, 3));
	pStrand->post(boost::bind(PrintNum, 4));
	pStrand->post(boost::bind(PrintNum, 5));
	pStrand->post(boost::bind(PrintNum, 6));

	pdlTimer->expires_from_now(boost::posix_time::seconds(2));
	//pdlTimer->async_wait(TimerHandler);
	pdlTimer->async_wait(boost::bind(TimerHandler, boost::asio::placeholders::error, pdlTimer));
	//pdlTimer->async_wait(pStrand->wrap(boost::bind(TimerHandler, boost::asio::placeholders::error, pdlTimer)));

	std::cin.get();

	pIOContext1->stop();

	tg.join_all();
}*/

/*boost::mutex global_stream_lock;

void WorkerThread(boost::shared_ptr< boost::asio::io_service > io_service)
{
	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id()
		<< "] Thread Start" << std::endl;
	global_stream_lock.unlock();

	while (true)
	{
		try
		{
			boost::system::error_code ec;
			io_service->run(ec);
			if (ec)
			{
				global_stream_lock.lock();
				std::cout << "[" << boost::this_thread::get_id()
					<< "] Error: " << ec << std::endl;
				global_stream_lock.unlock();
			}
			break;
		}
		catch (std::exception& ex)
		{
			global_stream_lock.lock();
			std::cout << "[" << boost::this_thread::get_id()
				<< "] Exception: " << ex.what() << std::endl;
			global_stream_lock.unlock();
		}
	}

	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id()
		<< "] Thread Finish" << std::endl;
	global_stream_lock.unlock();
}

void TimerHandler(
	const boost::system::error_code& error,
	boost::shared_ptr< boost::asio::deadline_timer > timer,
	boost::shared_ptr< boost::asio::io_service::strand > strand
)
{
	if (error)
	{
		global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id()
			<< "] Error: " << error << std::endl;
		global_stream_lock.unlock();
	}
	else
	{
		std::cout << "[" << boost::this_thread::get_id()
			<< "] TimerHandler " << std::endl;

		timer->expires_from_now(boost::posix_time::seconds(1));
		timer->async_wait(
			strand->wrap(boost::bind(&TimerHandler, boost::asio::placeholders::error, timer, strand))
		);
	}
}

void PrintNum(int x)
{
	std::cout << "[" << boost::this_thread::get_id()
		<< "] x: " << x << std::endl;
	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

int main(int argc, char* argv[])
{
	boost::shared_ptr< boost::asio::io_service > io_service(
		new boost::asio::io_service
	);
	boost::shared_ptr< boost::asio::io_service::work > work(
		new boost::asio::io_service::work(*io_service)
	);
	boost::shared_ptr< boost::asio::io_service::strand > strand(
		new boost::asio::io_service::strand(*io_service)
	);

	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id()
		<< "] Press [return] to exit." << std::endl;
	global_stream_lock.unlock();

	boost::thread_group worker_threads;
	for (int x = 0; x < 2; ++x)
	{
		worker_threads.create_thread(boost::bind(&WorkerThread, io_service));
	}

	boost::this_thread::sleep(boost::posix_time::seconds(1));

	strand->post(boost::bind(&PrintNum, 1));
	strand->post(boost::bind(&PrintNum, 2));
	strand->post(boost::bind(&PrintNum, 3));
	strand->post(boost::bind(&PrintNum, 4));
	strand->post(boost::bind(&PrintNum, 5));

	boost::shared_ptr< boost::asio::deadline_timer > timer(
		new boost::asio::deadline_timer(*io_service)
	);
	timer->expires_from_now(boost::posix_time::seconds(1));
	//timer->async_wait(
	//	strand->wrap(boost::bind(&TimerHandler, boost::asio::placeholders::error, timer, strand))
	//);
	timer->async_wait(boost::bind(&TimerHandler, boost::asio::placeholders::error, timer, strand));

	std::cin.get();

	io_service->stop();

	worker_threads.join_all();

	return 0;
}*/

// https://corecppil.github.io/Meetups/2018-05-28_Practical-C++Asio-Variant/Asynchronous_IO_with_boost.asio.pdf

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>

boost::mutex g_Mutex;

void StrandTest1(int i)
{
	g_Mutex.lock();
	std::cout << "StrandTest1(): " << boost::this_thread::get_id() << std::endl;
	std::cout << "i: " << i << std::endl;
	g_Mutex.unlock();

	boost::this_thread::sleep(boost::posix_time::seconds(20));

	g_Mutex.lock();
	std::cout << "StrandTest1() end: " << boost::this_thread::get_id() << std::endl;
	g_Mutex.unlock();
}

void StrandTest2(boost::shared_ptr<boost::asio::io_context>& pIOContext,
	boost::shared_ptr<boost::asio::io_context::strand>& pStrand)
{
	g_Mutex.lock();
	std::cout << "StrandTest2(): " << boost::this_thread::get_id() << std::endl;
	g_Mutex.unlock();

	boost::this_thread::sleep(boost::posix_time::seconds(2));
	pIOContext->post(pStrand->wrap(boost::bind(StrandTest1, 100)));

	g_Mutex.lock();
	std::cout << "StrandTest2() end: " << boost::this_thread::get_id() << std::endl;
	g_Mutex.unlock();
}

void WorkerThread1(boost::shared_ptr<boost::asio::io_context>& pIOContext)
{
	g_Mutex.lock();
	std::cout << "WorkerThread1(): " << boost::this_thread::get_id() << std::endl;
	g_Mutex.unlock();

	pIOContext->run();

	g_Mutex.lock();
	std::cout << "WorkerThread1() end: " << boost::this_thread::get_id() << std::endl;
	g_Mutex.unlock();
}

void _tmain()
{
	g_Mutex.lock();
	std::cout << "_tmain(): " << boost::this_thread::get_id() << std::endl;
	g_Mutex.unlock();

	boost::shared_ptr<boost::asio::io_context> pIOContext(new boost::asio::io_context());
	boost::shared_ptr<boost::asio::io_context::work>
		pWork(new boost::asio::io_context::work(*pIOContext));
	boost::shared_ptr<boost::asio::io_context::strand>
		pStrand(new boost::asio::io_context::strand(*pIOContext));

	boost::thread_group tg;

	for (int i = 0; i < 4; i++)
	{
		tg.create_thread(boost::bind(WorkerThread1, pIOContext));
	}
	boost::this_thread::sleep(boost::posix_time::seconds(1));

	//pStrand->post(boost::bind(StrandTest1, 1));
	//pStrand->post(boost::bind(StrandTest1, 2));
	//pStrand->post(boost::bind(StrandTest1, 3));
	//pStrand->post(boost::bind(StrandTest1, 4));
	//pStrand->post(boost::bind(StrandTest1, 5));
	//pStrand->post(boost::bind(StrandTest1, 6));
	//pStrand->post(boost::bind(StrandTest1, 7));
	//pStrand->post(boost::bind(StrandTest1, 8));
	//pStrand->post(boost::bind(StrandTest1, 9));

	pIOContext->post(boost::bind(StrandTest1, 1));
	pIOContext->post(boost::bind(StrandTest1, 2));
	pIOContext->post(boost::bind(StrandTest1, 3));
	pIOContext->post(boost::bind(StrandTest1, 4));
	pIOContext->post(boost::bind(StrandTest1, 5));
	pIOContext->post(boost::bind(StrandTest2, pIOContext, pStrand));
	pIOContext->post(boost::bind(StrandTest1, 6));
	pIOContext->post(boost::bind(StrandTest1, 7));
	pIOContext->post(boost::bind(StrandTest1, 8));
	pIOContext->post(boost::bind(StrandTest1, 9));
	pIOContext->post(boost::bind(StrandTest1, 10));
	pIOContext->post(boost::bind(StrandTest1, 11));
	pIOContext->post(boost::bind(StrandTest1, 12));
	pIOContext->post(boost::bind(StrandTest1, 14));
	pIOContext->post(boost::bind(StrandTest1, 15));
	pIOContext->post(boost::bind(StrandTest1, 16));
	pIOContext->post(boost::bind(StrandTest1, 17));
	pIOContext->post(boost::bind(StrandTest1, 18));
	pIOContext->post(boost::bind(StrandTest1, 19));
	pIOContext->post(boost::bind(StrandTest1, 20));
	//pStrand->post(boost::bind(StrandTest2, 10, pStrand)); // No effect

	std::cin.get();

	pIOContext->stop();

	tg.join_all();

	g_Mutex.lock();
	std::cout << "_tmain() end" << std::endl;
	g_Mutex.unlock();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

class sharedptrTest
{
public:
	sharedptrTest() { std::cout << "sharedptrTest" << std::endl; }
	void dosomething() { std::cout << "dosomething" << std::endl; }
	~sharedptrTest() { std::cout << "~sharedptrTest" << std::endl; }
};

boost::shared_ptr<sharedptrTest> p1;

void fun()
{
	boost::shared_ptr<sharedptrTest> psharedptrTest(new sharedptrTest());
	p1 = psharedptrTest;
}

void _tmain()
{
	// Thread safe test.
	boost::shared_ptr<sharedptrTest> pt1 = boost::make_shared<sharedptrTest>();
	boost::shared_ptr<sharedptrTest> pt2 = pt1;
	std::cout << pt1.use_count() << std::endl;
	std::cout << pt2.use_count() << std::endl;
	return;

	true;
	fun();
	boost::shared_ptr<sharedptrTest> p2;
	p2->dosomething();
	p2 = p1;
	std::cout << p2.use_count() << std::endl;
	std::cout << p1.use_count() << std::endl;
	if (p1)
	{
		std::cout << "p1 not null" << std::endl;
		p1 = nullptr;
	}
	std::cout << p1.use_count() << std::endl;
	p1.reset();
	if (!p1)
	{
		std::cout << "p1 null" << std::endl;
	}
	std::cout << p1.use_count() << std::endl;
	p2.reset();
	std::cout << p1.use_count() << std::endl;
	std::cin.get();
}*/

/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

void worker_thread(boost::shared_ptr<boost::asio::io_context> pIOContext)
{
	std::cout << "worker_thread" << std::endl;
	pIOContext->run();
	std::cout << "worker_thread end" << std::endl;
}

void _tmain()
{
	boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());
	boost::shared_ptr<boost::asio::io_context::work> pWorker1(new boost::asio::io_context::work(*pIOContext1));

	boost::thread_group tg;

	//tg.create_thread(boost::bind(worker_thread, pIOContext1));
	tg.create_thread(boost::bind(&boost::asio::io_context::run, pIOContext1));

	std::cin.get();

	pIOContext1->stop();

	tg.join_all();
}*/

/*// https://stackoverflow.com/questions/25628704/enable-shared-from-this-why-the-crash/25628969
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

class Person : public std::enable_shared_from_this<Person> {
private:
	std::string name;
public:
	Person(const std::string& name) : name(name) { std::cout << "Person" << std::endl; }
	~Person() { std::cout << "~Person" << std::endl; }
	std::string getName() const { return name; }
	void introduce() const;
};

void displayName(std::shared_ptr<const Person> person) {
	std::cout << "Name is " << person->getName() << "." << std::endl;
}

void Person::introduce() const {
	displayName(this->shared_from_this());
}

int main() {
	Person* bob = new Person("Bob");
	//bob->introduce();  // Crash here.  Why?
	bob = nullptr;
}*/

/*// https://stackoverflow.com/questions/25628704/enable-shared-from-this-why-the-crash/25628969
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

class Person : public std::enable_shared_from_this<Person> {
private:
	std::string name;
public:
	Person(const std::string& name) : name(name) { std::cout << "Person" << std::endl; }
	~Person() { std::cout << "~Person" << std::endl; }
	std::string getName() const { return name; }
	void introduce() const;
};

void displayName(std::shared_ptr<const Person> person) {
	std::cout << "Name is " << person->getName() << "." << std::endl;
}

void Person::introduce() const {
	displayName(this->shared_from_this());
}

int main() {
	Person* bob = new Person("Bob");
	Person* tmp = NULL;
	std::shared_ptr<Person> pPerson(bob);
	std::cout << pPerson.use_count() << std::endl;
	std::shared_ptr<Person> pPerson1;
	{
		std::shared_ptr<Person>& pPerson2(pPerson);
		std::cout << pPerson2.use_count() << std::endl;
		//pPerson2.reset();
		//std::cout << pPerson2.use_count() << std::endl;
		//std::shared_ptr<Person> pPerson1(pPerson2);
		pPerson1 = pPerson2;
		std::cout << pPerson1.use_count() << std::endl;
	}
	//delete bob;
	std::cout << pPerson1.use_count() << std::endl;
	std::cout << pPerson.use_count() << std::endl;
	pPerson.reset();
	std::cout << "Last " << pPerson1.use_count() << std::endl;
	std::cout << "Last " << pPerson.use_count() << std::endl;
	pPerson1.reset();
	std::cout << "Last " << pPerson1.use_count() << std::endl;
	std::cout << "Last " << pPerson.use_count() << std::endl;
	//pPerson = std::make_shared<Person>("Alice");
	//pPerson = bob; //Wrong
	//bob->introduce();  // Crash here.  Why?
	//bob = nullptr;
}*/

// https://stackoverflow.com/questions/25628704/enable-shared-from-this-why-the-crash/25628969
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

class Person : public std::enable_shared_from_this<Person> {
private:
	std::string name;
public:
	Person(const std::string& name) : name(name) { std::cout << "Person" << std::endl; }
	~Person() { std::cout << "~Person" << std::endl; }
	std::string getName() const { return name; }
	void introduce() const;
};

void displayName(std::shared_ptr<const Person> person) {
	std::cout << "Name is " << person->getName() << "." << std::endl;
}

void Person::introduce() const {
	displayName(this->shared_from_this());
}

int main() {
	Person* bob = new Person("Bob");
	Person* tmp = NULL;
	boost::shared_ptr<Person> pPerson(bob);
	std::cout << pPerson.use_count() << std::endl;
	{
		boost::shared_ptr<Person>& pPerson1(pPerson);
		std::cout << pPerson.use_count() << std::endl;
		std::cout << pPerson1.use_count() << std::endl;
		pPerson1.reset();
		std::cout << pPerson.use_count() << std::endl;
		std::cout << pPerson1.use_count() << std::endl;
	}
	std::cout << pPerson.use_count() << std::endl;
	pPerson.reset();
	std::cout << pPerson.use_count() << std::endl;
	//pPerson = std::make_shared<Person>("Alice");
	//pPerson = bob; //Wrong
	//bob->introduce();  // Crash here.  Why?
	//bob = nullptr;
}

