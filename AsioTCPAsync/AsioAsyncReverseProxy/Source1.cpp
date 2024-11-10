/*
//-----------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Winerror.h>

#include <conio.h>

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind/placeholders.hpp> 
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/detail/atomic.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;



using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;

//-----------------------------------------------------------------------------

class Hive;
class Acceptor;
class Connection;

//-----------------------------------------------------------------------------

class Connection : public boost::enable_shared_from_this< Connection >
{
	friend class Acceptor;
	friend class Hive;

private:
	boost::shared_ptr< Hive > m_hive;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::strand m_io_strand;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	std::vector< uint8_t > m_recv_buffer;
	std::list< int32_t > m_pending_recvs;
	std::list< std::vector< uint8_t > > m_pending_sends;
	int32_t m_receive_buffer_size;
	int32_t m_timer_interval;
	volatile uint32_t m_error_state;

protected:
	Connection(boost::shared_ptr< Hive > hive);
	virtual ~Connection();

private:
	Connection(const Connection& rhs);
	Connection& operator =(const Connection& rhs);
	void StartSend();
	void StartRecv(int32_t total_bytes);
	void StartTimer();
	void StartError(const boost::system::error_code& error);
	void DispatchSend(std::vector< uint8_t > buffer);
	void DispatchRecv(int32_t total_bytes);
	void DispatchTimer(const boost::system::error_code& error);
	void HandleConnect(const boost::system::error_code& error);
	void HandleSend(const boost::system::error_code& error, std::list< std::vector< uint8_t > >::iterator itr);
	void HandleRecv(const boost::system::error_code& error, int32_t actual_bytes);
	void HandleTimer(const boost::system::error_code& error);

private:
	// Called when the connection has successfully connected to the local
	// host.
	virtual void OnAccept(const std::string& host, uint16_t port) = 0;

	// Called when the connection has successfully connected to the remote
	// host.
	virtual void OnConnect(const std::string& host, uint16_t port) = 0;

	// Called when data has been sent by the connection.
	virtual void OnSend(const std::vector< uint8_t >& buffer) = 0;

	// Called when data has been received by the connection.
	virtual void OnRecv(std::vector< uint8_t >& buffer) = 0;

	// Called on each timer event.
	virtual void OnTimer(const boost::posix_time::time_duration& delta) = 0;

	// Called when an error is encountered.
	virtual void OnError(const boost::system::error_code& error) = 0;

public:
	// Returns the Hive object.
	boost::shared_ptr< Hive > GetHive();

	// Returns the socket object.
	boost::asio::ip::tcp::socket& GetSocket();

	// Returns the strand object.
	boost::asio::strand& GetStrand();

	// Sets the application specific receive buffer size used. For stream
	// based protocols such as HTTP, you want this to be pretty large, like
	// 64kb. For packet based protocols, then it will be much smaller,
	// usually 512b - 8kb depending on the protocol. The default value is
	// 4kb.
	void SetReceiveBufferSize(int32_t size);

	// Returns the size of the receive buffer size of the current object.
	int32_t GetReceiveBufferSize() const;

	// Sets the timer interval of the object. The interval is changed after
	// the next update is called.
	void SetTimerInterval(int32_t timer_interval_ms);

	// Returns the timer interval of the object.
	int32_t GetTimerInterval() const;

	// Returns true if this object has an error associated with it.
	bool HasError();

	// Binds the socket to the specified interface.
	void Bind(const std::string& ip, uint16_t port);

	// Starts an a/synchronous connect.
	void Connect(const std::string& host, uint16_t port);

	// Posts data to be sent to the connection.
	void Send(const std::vector< uint8_t >& buffer);

	// Posts a recv for the connection to process. If total_bytes is 0, then
	// as many bytes as possible up to GetReceiveBufferSize() will be
	// waited for. If Recv is not 0, then the connection will wait for exactly
	// total_bytes before invoking OnRecv.
	void Recv(int32_t total_bytes = 0);

	// Posts an asynchronous disconnect event for the object to process.
	void Disconnect();
};

//-----------------------------------------------------------------------------

class Acceptor : public boost::enable_shared_from_this< Acceptor >
{
	friend class Hive;

private:
	boost::shared_ptr< Hive > m_hive;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::strand m_io_strand;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	int32_t m_timer_interval;
	volatile uint32_t m_error_state;

private:
	Acceptor(const Acceptor& rhs);
	Acceptor& operator =(const Acceptor& rhs);
	void StartTimer();
	void StartError(const boost::system::error_code& error);
	void DispatchAccept(boost::shared_ptr< Connection > connection);
	void HandleTimer(const boost::system::error_code& error);
	void HandleAccept(const boost::system::error_code& error, boost::shared_ptr< Connection > connection);

protected:
	Acceptor(boost::shared_ptr< Hive > hive);
	virtual ~Acceptor();

private:
	// Called when a connection has connected to the server. This function
	// should return true to invoke the connection's OnAccept function if the
	// connection will be kept. If the connection will not be kept, the
	// connection's Disconnect function should be called and the function
	// should return false.
	virtual bool OnAccept(boost::shared_ptr< Connection > connection, const std::string& host, uint16_t port) = 0;

	// Called on each timer event.
	virtual void OnTimer(const boost::posix_time::time_duration& delta) = 0;

	// Called when an error is encountered. Most typically, this is when the
	// acceptor is being closed via the Stop function or if the Listen is
	// called on an address that is not available.
	virtual void OnError(const boost::system::error_code& error) = 0;

public:
	// Returns the Hive object.
	boost::shared_ptr< Hive > GetHive();

	// Returns the acceptor object.
	boost::asio::ip::tcp::acceptor& GetAcceptor();

	// Returns the strand object.
	boost::asio::strand& GetStrand();

	// Sets the timer interval of the object. The interval is changed after
	// the next update is called. The default value is 1000 ms.
	void SetTimerInterval(int32_t timer_interval_ms);

	// Returns the timer interval of the object.
	int32_t GetTimerInterval() const;

	// Returns true if this object has an error associated with it.
	bool HasError();

public:
	// Begin listening on the specific network interface.
	void Listen(const std::string& host, const uint16_t& port);

	// Posts the connection to the listening interface. The next client that
	// connections will be given this connection. If multiple calls to Accept
	// are called at a time, then they are accepted in a FIFO order.
	void Accept(boost::shared_ptr< Connection > connection);

	// Stop the Acceptor from listening.
	void Stop();
};

//-----------------------------------------------------------------------------

class Hive : public boost::enable_shared_from_this< Hive >
{
private:
	boost::asio::io_service m_io_service;
	boost::shared_ptr< boost::asio::io_service::work > m_work_ptr;
	volatile uint32_t m_shutdown;

private:
	Hive(const Hive& rhs);
	Hive& operator =(const Hive& rhs);

public:
	Hive();
	virtual ~Hive();

	// Returns the io_service of this object.
	boost::asio::io_service& GetService();

	// Returns true if the Stop function has been called.
	bool HasStopped();

	// Polls the networking subsystem once from the current thread and
	// returns.
	void Poll();

	// Runs the networking system on the current thread. This function blocks
	// until the networking system is stopped, so do not call on a single
	// threaded application with no other means of being able to call Stop
	// unless you code in such logic.
	void Run();

	// Stops the networking system. All work is finished and no more
	// networking interactions will be possible afterwards until Reset is called.
	void Stop();

	// Restarts the networking system after Stop as been called. A new work
	// object is created ad the shutdown flag is cleared.
	void Reset();
};

//-----------------------------------------------------------------------------

//network.cpp
//#include "network.h"

//-----------------------------------------------------------------------------

Hive::Hive()
	: m_work_ptr(new boost::asio::io_service::work(m_io_service)), m_shutdown(0)
{
}

Hive::~Hive()
{
}

boost::asio::io_service& Hive::GetService()
{
	return m_io_service;
}

bool Hive::HasStopped()
{
	return (boost::interprocess::detail::atomic_cas32(&m_shutdown, 1, 1) == 1);
}

void Hive::Poll()
{
	m_io_service.poll();
}

void Hive::Run()
{
	m_io_service.run();
}

void Hive::Stop()
{
	if (boost::interprocess::detail::atomic_cas32(&m_shutdown, 1, 0) == 0)
	{
		m_work_ptr.reset();
		m_io_service.run();
		m_io_service.stop();
	}
}

void Hive::Reset()
{
	if (boost::interprocess::detail::atomic_cas32(&m_shutdown, 0, 1) == 1)
	{
		m_io_service.reset();
		m_work_ptr.reset(new boost::asio::io_service::work(m_io_service));
	}
}

//-----------------------------------------------------------------------------

Acceptor::Acceptor(boost::shared_ptr< Hive > hive)
	: m_hive(hive), m_acceptor(hive->GetService()), m_io_strand(hive->GetService()),
	m_timer(hive->GetService()), m_timer_interval(1000), m_error_state(0)
{
}

Acceptor::~Acceptor()
{
}

void Acceptor::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&Acceptor::HandleTimer, shared_from_this(), _1)));
}

void Acceptor::StartError(const boost::system::error_code& error)
{
	if (boost::interprocess::detail::atomic_cas32(&m_error_state, 1, 0) == 0)
	{
		boost::system::error_code ec;
		m_acceptor.cancel(ec);
		m_acceptor.close(ec);
		m_timer.cancel(ec);
		OnError(error);
	}
}

void Acceptor::DispatchAccept(boost::shared_ptr< Connection > connection)
{
	m_acceptor.async_accept(connection->GetSocket(), connection->GetStrand().wrap(boost::bind(&Acceptor::HandleAccept, shared_from_this(), _1, connection)));
}

void Acceptor::HandleTimer(const boost::system::error_code& error)
{
	if (error || HasError() || m_hive->HasStopped())
	{
		StartError(error);
	}
	else
	{
		OnTimer(boost::posix_time::microsec_clock::local_time() - m_last_time);
		StartTimer();
	}
}

void Acceptor::HandleAccept(const boost::system::error_code& error, boost::shared_ptr< Connection > connection)
{
	if (error || HasError() || m_hive->HasStopped())
	{
		connection->StartError(error);
	}
	else
	{
		if (connection->GetSocket().is_open())
		{
			connection->StartTimer();
			if (OnAccept(connection, connection->GetSocket().remote_endpoint().address().to_string(), connection->GetSocket().remote_endpoint().port()))
			{
				connection->OnAccept(m_acceptor.local_endpoint().address().to_string(), m_acceptor.local_endpoint().port());
			}
		}
		else
		{
			StartError(error);
		}
	}
}

void Acceptor::Stop()
{
	m_io_strand.post(boost::bind(&Acceptor::HandleTimer, shared_from_this(), boost::asio::error::connection_reset));
}

void Acceptor::Accept(boost::shared_ptr< Connection > connection)
{
	m_io_strand.post(boost::bind(&Acceptor::DispatchAccept, shared_from_this(), connection));
}

void Acceptor::Listen(const std::string& host, const uint16_t& port)
{
	boost::asio::ip::tcp::resolver resolver(m_hive->GetService());
	boost::asio::ip::tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
	m_acceptor.bind(endpoint);
	m_acceptor.listen(boost::asio::socket_base::max_connections);
	StartTimer();
}

boost::shared_ptr< Hive > Acceptor::GetHive()
{
	return m_hive;
}

boost::asio::ip::tcp::acceptor& Acceptor::GetAcceptor()
{
	return m_acceptor;
}

int32_t Acceptor::GetTimerInterval() const
{
	return m_timer_interval;
}

void Acceptor::SetTimerInterval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}

bool Acceptor::HasError()
{
	return (boost::interprocess::detail::atomic_cas32(&m_error_state, 1, 1) == 1);
}

//-----------------------------------------------------------------------------

Connection::Connection(boost::shared_ptr< Hive > hive)
	: m_hive(hive), m_socket(hive->GetService()), m_io_strand(hive->GetService()), m_timer(hive->GetService()), m_receive_buffer_size(4096), m_timer_interval(1000), m_error_state(0)
{
}

Connection::~Connection()
{
}

void Connection::Bind(const std::string& ip, uint16_t port)
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
	m_socket.open(endpoint.protocol());
	m_socket.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
	m_socket.bind(endpoint);
}

void Connection::StartSend()
{
	if (!m_pending_sends.empty())
	{
		boost::asio::async_write(m_socket, boost::asio::buffer(m_pending_sends.front()), m_io_strand.wrap(boost::bind(&Connection::HandleSend, shared_from_this(), boost::asio::placeholders::error, m_pending_sends.begin())));
	}
}

void Connection::StartRecv(int32_t total_bytes)
{
	if (total_bytes > 0)
	{
		m_recv_buffer.resize(total_bytes);
		boost::asio::async_read(m_socket, boost::asio::buffer(m_recv_buffer), m_io_strand.wrap(boost::bind(&Connection::HandleRecv, shared_from_this(), _1, _2)));
	}
	else
	{
		m_recv_buffer.resize(m_receive_buffer_size);
		m_socket.async_read_some(boost::asio::buffer(m_recv_buffer), m_io_strand.wrap(boost::bind(&Connection::HandleRecv, shared_from_this(), _1, _2)));
	}
}

void Connection::StartTimer()
{
	m_last_time = boost::posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&Connection::DispatchTimer, shared_from_this(), _1)));
}

void Connection::StartError(const boost::system::error_code& error)
{
	if (boost::interprocess::detail::atomic_cas32(&m_error_state, 1, 0) == 0)
	{
		boost::system::error_code ec;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		m_socket.close(ec);
		m_timer.cancel(ec);
		OnError(error);
	}
}

void Connection::HandleConnect(const boost::system::error_code& error)
{
	if (error || HasError() || m_hive->HasStopped())
	{
		StartError(error);
	}
	else
	{
		if (m_socket.is_open())
		{
			OnConnect(m_socket.remote_endpoint().address().to_string(), m_socket.remote_endpoint().port());
		}
		else
		{
			StartError(error);
		}
	}
}

void Connection::HandleSend(const boost::system::error_code& error, std::list< std::vector< uint8_t > >::iterator itr)
{
	if (error || HasError() || m_hive->HasStopped())
	{
		StartError(error);
	}
	else
	{
		OnSend(*itr);
		m_pending_sends.erase(itr);
		StartSend();
	}
}

void Connection::HandleRecv(const boost::system::error_code& error, int32_t actual_bytes)
{
	if (error || HasError() || m_hive->HasStopped())
	{
		StartError(error);
	}
	else
	{
		m_recv_buffer.resize(actual_bytes);
		OnRecv(m_recv_buffer);
		m_pending_recvs.pop_front();
		if (!m_pending_recvs.empty())
		{
			StartRecv(m_pending_recvs.front());
		}
	}
}

void Connection::HandleTimer(const boost::system::error_code& error)
{
	if (error || HasError() || m_hive->HasStopped())
	{
		StartError(error);
	}
	else
	{
		OnTimer(boost::posix_time::microsec_clock::local_time() - m_last_time);
		StartTimer();
	}
}

void Connection::DispatchSend(std::vector< uint8_t > buffer)
{
	bool should_start_send = m_pending_sends.empty();
	m_pending_sends.push_back(buffer);
	if (should_start_send)
	{
		StartSend();
	}
}

void Connection::DispatchRecv(int32_t total_bytes)
{
	bool should_start_receive = m_pending_recvs.empty();
	m_pending_recvs.push_back(total_bytes);
	if (should_start_receive)
	{
		StartRecv(total_bytes);
	}
}

void Connection::DispatchTimer(const boost::system::error_code& error)
{
	m_io_strand.post(boost::bind(&Connection::HandleTimer, shared_from_this(), error));
}

void Connection::Connect(const std::string& host, uint16_t port)
{
	boost::system::error_code ec;
	boost::asio::ip::tcp::resolver resolver(m_hive->GetService());
	boost::asio::ip::tcp::resolver::query query(host, boost::lexical_cast<std::string>(port));
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	m_socket.async_connect(*iterator, m_io_strand.wrap(boost::bind(&Connection::HandleConnect, shared_from_this(), _1)));
	StartTimer();
}

void Connection::Disconnect()
{
	m_io_strand.post(boost::bind(&Connection::HandleTimer, shared_from_this(), boost::asio::error::connection_reset));
}

void Connection::Recv(int32_t total_bytes)
{
	m_io_strand.post(boost::bind(&Connection::DispatchRecv, shared_from_this(), total_bytes));
}

void Connection::Send(const std::vector< uint8_t >& buffer)
{
	m_io_strand.post(boost::bind(&Connection::DispatchSend, shared_from_this(), buffer));
}

boost::asio::ip::tcp::socket& Connection::GetSocket()
{
	return m_socket;
}

boost::asio::strand& Connection::GetStrand()
{
	return m_io_strand;
}

boost::shared_ptr< Hive > Connection::GetHive()
{
	return m_hive;
}

void Connection::SetReceiveBufferSize(int32_t size)
{
	m_receive_buffer_size = size;
}

int32_t Connection::GetReceiveBufferSize() const
{
	return m_receive_buffer_size;
}

int32_t Connection::GetTimerInterval() const
{
	return m_timer_interval;
}

void Connection::SetTimerInterval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}

bool Connection::HasError()
{
	return (boost::interprocess::detail::atomic_cas32(&m_error_state, 1, 1) == 1);
}

//-----------------------------------------------------------------------------

//#include "network.h"

boost::mutex global_stream_lock;

class MyConnection : public Connection
{
private:

private:
	void OnAccept(const std::string& host, uint16_t port)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnConnect(const std::string& host, uint16_t port)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnSend(const std::vector< uint8_t >& buffer)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for (size_t x = 0; x < buffer.size(); ++x)
		{
			std::cout << std::hex << std::setfill('0') <<
				std::setw(2) << (int)buffer[x] << " ";
			if ((x + 1) % 16 == 0)
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();
	}

	void OnRecv(std::vector< uint8_t >& buffer)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for (size_t x = 0; x < buffer.size(); ++x)
		{
			std::cout << std::hex << std::setfill('0') <<
				std::setw(2) << (int)buffer[x] << " ";
			if ((x + 1) % 16 == 0)
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();

		// Echo the data back
		Send(buffer);
	}

	void OnTimer(const boost::posix_time::time_duration& delta)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError(const boost::system::error_code& error)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyConnection(boost::shared_ptr< Hive > hive)
		: Connection(hive)
	{
	}

	~MyConnection()
	{
	}
};

class MyAcceptor : public Acceptor
{
private:

private:
	bool OnAccept(boost::shared_ptr< Connection > connection, const std::string& host, uint16_t port)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		return true;
	}

	void OnTimer(const boost::posix_time::time_duration& delta)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError(const boost::system::error_code& error)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyAcceptor(boost::shared_ptr< Hive > hive)
		: Acceptor(hive)
	{
	}

	~MyAcceptor()
	{
	}
};

int main(int argc, char* argv[])
{
	boost::shared_ptr< Hive > hive(new Hive());

	boost::shared_ptr< MyAcceptor > acceptor(new MyAcceptor(hive));
	acceptor->Listen("127.0.0.1", 7777);

	boost::shared_ptr< MyConnection > connection(new MyConnection(hive));
	acceptor->Accept(connection);

	while (!_kbhit())
	{
		hive->Poll();
		Sleep(1);
	}

	hive->Stop();

	return 0;
}

//#include "network.h"

boost::mutex global_stream_lock;

class MyConnection : public Connection
{
private:

private:
	void OnAccept(const std::string& host, uint16_t port)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnConnect(const std::string& host, uint16_t port)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();

		std::string str = "GET / HTTP/1.0\r\n\r\n";

		std::vector< uint8_t > request;
		std::copy(str.begin(), str.end(), std::back_inserter(request));
		Send(request);
	}

	void OnSend(const std::vector< uint8_t >& buffer)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for (size_t x = 0; x < buffer.size(); ++x)
		{
			std::cout << std::hex << std::setfill('0') <<
				std::setw(2) << (int)buffer[x] << " ";
			if ((x + 1) % 16 == 0)
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();
	}

	void OnRecv(std::vector< uint8_t >& buffer)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for (size_t x = 0; x < buffer.size(); ++x)
		{
			std::cout << std::hex << std::setfill('0') <<
				std::setw(2) << (int)buffer[x] << " ";
			if ((x + 1) % 16 == 0)
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnTimer(const boost::posix_time::time_duration& delta)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError(const boost::system::error_code& error)
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyConnection(boost::shared_ptr< Hive > hive)
		: Connection(hive)
	{
	}

	~MyConnection()
	{
	}
};

int main(int argc, char* argv[])
{
	boost::shared_ptr< Hive > hive(new Hive());

	boost::shared_ptr< MyConnection > connection(new MyConnection(hive));
	connection->Connect("www.google.com", 80);

	while (!_kbhit())
	{
		hive->Poll();
		Sleep(1);
	}

	hive->Stop();

	return 0;
}*/