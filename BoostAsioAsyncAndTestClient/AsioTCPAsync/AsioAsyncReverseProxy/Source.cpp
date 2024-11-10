/*
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <Winerror.h>

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

char szServerBuffer[1024];
char szClientBuffer[1024];

boost::mutex g_Mutex;

void writeclient_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	g_Mutex.lock();
	cout << "Client Sent: " << bytes_transferred << endl;
	g_Mutex.unlock();
}

void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	g_Mutex.lock();
	cout << "Server Sent: " << bytes_transferred << endl;
	g_Mutex.unlock();
}

void readclient_handler(const boost::system::error_code& ec, std::size_t bytes_transferred,
	boost::shared_ptr<tcp::socket>& pClientSocket, boost::shared_ptr<tcp::socket>& pPeerSocket)
{
	if (!ec || bytes_transferred != 0)
	{
		g_Mutex.lock();
		cout << "Client Received: " << bytes_transferred << endl;
		g_Mutex.unlock();
		pPeerSocket->async_write_some(boost::asio::buffer(szClientBuffer, bytes_transferred),
			boost::bind(write_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

		pClientSocket->async_read_some(boost::asio::buffer(szClientBuffer, sizeof(szClientBuffer)),
			boost::bind(readclient_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
				pClientSocket, pPeerSocket));
	}
	else
	{
		g_Mutex.lock();
		cout << "Client receive error: " << ec << endl;
		g_Mutex.unlock();
	}
}

void connect_handler(const boost::system::error_code& ec, boost::shared_ptr<tcp::socket>& pClientSocket,
	boost::shared_ptr<tcp::socket>& pPeerSocket)
{
	if (!ec)
	{
		g_Mutex.lock();
		cout << "CLient connected:" << endl;
		g_Mutex.unlock();
		pClientSocket->async_read_some(boost::asio::buffer(szClientBuffer, sizeof(szClientBuffer)),
			boost::bind(readclient_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
				pClientSocket, pPeerSocket));
	}
	else
	{
		g_Mutex.lock();
		cout << "Client error: " << ec << endl;
		g_Mutex.unlock();
	}
}

void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred,
	boost::shared_ptr<tcp::socket>& pPeerSocket, boost::shared_ptr<tcp::socket>& pClientSocket)
{
	if (!ec || bytes_transferred != 0)
	{
		g_Mutex.lock();
		cout << "Server Received: " << bytes_transferred << endl;
		g_Mutex.unlock();

		pClientSocket->async_write_some(boost::asio::buffer(szServerBuffer, bytes_transferred),
			boost::bind(writeclient_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

		pPeerSocket->async_read_some(boost::asio::buffer(szServerBuffer, sizeof(szServerBuffer)),
			boost::bind(read_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
				pPeerSocket, pClientSocket));
	}
	else
	{
		g_Mutex.lock();
		cout << "Server receive error: " << ec << endl;
		g_Mutex.unlock();
	}
}

void accept_handler(const boost::system::error_code& ec, boost::shared_ptr<io_context>& pIOContext,
	boost::shared_ptr<tcp::acceptor>& pAcceptor, boost::shared_ptr<tcp::socket>& pPeerSocket)
{
	if (!ec)
	{
		g_Mutex.lock();
		cout << "Server accepted:" << endl;
		g_Mutex.unlock();
		boost::shared_ptr<tcp::socket> pNewPeerSocket(new tcp::socket(*pIOContext));
		pAcceptor->async_accept(*pNewPeerSocket,
			boost::bind(accept_handler, boost::asio::placeholders::error, pIOContext, pAcceptor, pNewPeerSocket));

		// Client
		boost::shared_ptr<tcp::socket> pCientSocket1(new tcp::socket(*pIOContext));
		boost::shared_ptr<tcp::resolver> pClientResolver(new tcp::resolver(*pIOContext));
		boost::shared_ptr<tcp::resolver::query> pClientResolverQuery(
			new tcp::resolver::query("127.0.0.1", boost::lexical_cast<string>(16365)));
		tcp::resolver::iterator itrEndPoint = pClientResolver->resolve(*pClientResolverQuery);
		pCientSocket1->async_connect(*itrEndPoint, boost::bind(connect_handler, boost::asio::placeholders::error,
			pCientSocket1, pPeerSocket));

		Sleep(2000);

		pPeerSocket->async_read_some(boost::asio::buffer(szServerBuffer, sizeof(szServerBuffer)),
			boost::bind(read_handler, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
				pPeerSocket, pCientSocket1));
	}
	else
	{
		g_Mutex.lock();
		cout << "Server accept error: " << ec << endl;
		g_Mutex.unlock();
	}
}

void ReverProxyThread(boost::shared_ptr<io_context>& pIOContext)
{
	while (1)
	{
		try
		{
			boost::system::error_code ec;
			pIOContext->run(ec);
			break;
		}
		catch (std::runtime_error& ex)
		{

		}
	}
}

void _tmain()
{
	// IOContext
	boost::shared_ptr<io_context> pIOContext1(new io_context());
	boost::shared_ptr<io_context::strand> pStrand(new io_context::strand(*pIOContext1));

	// Worker Thread
	boost::shared_ptr<io_context::work> pWork(new io_context::work(*pIOContext1));
	boost::shared_ptr<boost::thread_group> pTG(new boost::thread_group());
	for (int i = 0; i < 2; i++)
	{
		pTG->create_thread(boost::bind(ReverProxyThread, pIOContext1));
	}

	// Resolver
	boost::shared_ptr<tcp::resolver> pResolver(new tcp::resolver(*pIOContext1));
	boost::shared_ptr<tcp::resolver::query>
		pQuery1(new tcp::resolver::query(tcp::v4(), boost::lexical_cast<string>(5800)));
	tcp::resolver::iterator pIter1 = pResolver->resolve(*pQuery1);
	boost::shared_ptr<tcp::acceptor> pAcceptor(new tcp::acceptor(*pIOContext1));

	// Acceptor
	tcp::endpoint Endpoint = (*pIter1);
	pAcceptor->open(Endpoint.protocol());
	pAcceptor->bind(*pIter1);
	pAcceptor->listen();
	boost::shared_ptr<tcp::socket> pPeerSocket(new tcp::socket(*pIOContext1));
	pAcceptor->async_accept(*pPeerSocket,
		boost::bind(accept_handler, boost::asio::placeholders::error, pIOContext1, pAcceptor, pPeerSocket));


	cin.get();

	system::error_code ec;
	pPeerSocket->close(ec);
	pPeerSocket->shutdown(tcp::socket::shutdown_both, ec);

	pIOContext1->stop();

	pTG->join_all();
}*/

// Working
/*#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

class XBOOSTAsyncSocket : public boost::enable_shared_from_this<XBOOSTAsyncSocket>
{
public:
	XBOOSTAsyncSocket(boost::shared_ptr<boost::asio::io_context>& pIOContext)
		: m_pSocket(new boost::asio::ip::tcp::socket(*pIOContext))
	{
		std::cout << "XBOOSTAsyncSocket" << std::endl;
		m_arrBuffer.empty();
	}

	virtual ~XBOOSTAsyncSocket()
	{
		std::cout << "~XBOOSTAsyncSocket" << std::endl;
		boost::system::error_code ec;
		m_pSocket->close(ec);
		m_pSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	}

	boost::shared_ptr<boost::asio::ip::tcp::socket>& operator ()()
	{
		return m_pSocket;
	}

	//void Connect(std::string szHost, int nPort)
	//{
	//	m_pSocket->async_connect(*GetEndpoint(szHost, nPort),
	//		boost::bind(&XBOOSTAsyncSocket::ConnectHandler, this, boost::asio::placeholders::error));
	//}

	//boost::shared_ptr<boost::asio::ip::tcp::endpoint> GetEndpoint(std::string szHost, int nPort)
	//{
	//	boost::shared_ptr<boost::asio::ip::tcp::resolver>
	//		pResolver(new boost::asio::ip::tcp::resolver(m_pSocket->get_executor()));
	//	boost::shared_ptr<boost::asio::ip::tcp::resolver::query>
	//		pQuery(new boost::asio::ip::tcp::resolver::query(szHost, boost::lexical_cast<std::string>(nPort)));
	//	boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint =
	//		boost::make_shared<boost::asio::ip::tcp::endpoint>(*pResolver->resolve(*pQuery));

	//	return pEndpoint;
	//}

	//void ConnectHandler(const boost::system::error_code& ec)
	//{
	//	m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
	//		boost::bind(&XBOOSTAsyncSocket::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	//}

	void Read()
	{
		std::cout << "Read" << std::endl;

		m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
			boost::bind(&XBOOSTAsyncSocket::read_handler, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	//Write(boost::shared_ptr<boost::asio::mutable_buffer>& pMutableBuffer)
	//{
	//	m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
	//		boost::bind(&XBOOSTAsyncSocket::read_handler, this, boost::asio::placeholders::error,
	//			boost::asio::placeholders::bytes_transferred, pMutableBuffer));
	//}

	void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		std::cout << "read_handler" << std::endl;
		if (!ec)
		{
			// Do Write
			std::cout << m_arrBuffer[0] << std::endl;

			m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
				boost::bind(&XBOOSTAsyncSocket::read_handler, shared_from_this(), boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
			std::cout << ec << std::endl;
	}

	//void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred,
	//	boost::shared_ptr<boost::asio::mutable_buffer>& pMutableBuffer)
	//{
	//	if (!ec)
	//	{
	//		// Do Write

	//		m_pSocket->async_write_some(pMutableBuffer,
	//			boost::bind(&XBOOSTAsyncSocket::write_handler, this, boost::asio::placeholders::error,
	//				boost::asio::placeholders::bytes_transferred, pMutableBuffer));
	//	}

	//}
private:
	boost::shared_ptr<boost::asio::ip::tcp::socket> m_pSocket;
	std::array<char, 4096> m_arrBuffer;
};

class XBOOSTAsyncServer
{
public:
	XBOOSTAsyncServer(int nPort) : m_pIOContext(new boost::asio::io_context()),
		m_pThreadGroup(new boost::thread_group()),
		m_pAcceptor(new boost::asio::ip::tcp::acceptor(*m_pIOContext, *GetEndpoint(nPort)))
	{
	}

	virtual ~XBOOSTAsyncServer()
	{
		m_pIOContext->stop();
	}

	void run()
	{
		m_pThreadGroup->create_thread(boost::bind(&boost::asio::io_context::run, m_pIOContext));
	}

	boost::shared_ptr<boost::asio::ip::tcp::endpoint> GetEndpoint(int nPort)
	{
		boost::shared_ptr<boost::asio::ip::tcp::resolver>
			pResolver(new boost::asio::ip::tcp::resolver(*m_pIOContext));
		boost::shared_ptr<boost::asio::ip::tcp::resolver::query>
			pQuery(new boost::asio::ip::tcp::resolver::query(boost::asio::ip::tcp::v4(),
				boost::lexical_cast<std::string>(nPort)));
		boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint =
			boost::make_shared<boost::asio::ip::tcp::endpoint>(*pResolver->resolve(*pQuery));

		return pEndpoint;
	}

	void Accept(int nPort)
	{
		std::cout << "Accept" << std::endl;
		//XBOOSTAsyncSocket xBOOSTAsyncPeerSocket(m_pIOContext);
		//boost::shared_ptr<XBOOSTAsyncSocket> pxBOOSTAsyncPeerSocket(new XBOOSTAsyncSocket(m_pIOContext));
		boost::shared_ptr<XBOOSTAsyncSocket> pxBOOSTAsyncPeerSocket =
			boost::make_shared<XBOOSTAsyncSocket>(m_pIOContext);
		m_pAcceptor->async_accept(*(*pxBOOSTAsyncPeerSocket)(), *GetEndpoint(nPort),
			boost::bind(&XBOOSTAsyncServer::AcceptHandler, this, boost::asio::placeholders::error,
				pxBOOSTAsyncPeerSocket, nPort));
		//return pxBOOSTAsyncPeerSocket;
	}

	void AcceptHandler(const boost::system::error_code& ec, boost::shared_ptr<XBOOSTAsyncSocket> pXPeerSocket, int nPort)
	{
		std::cout << "AcceptHandler" << std::endl;
		//boost::shared_ptr<XBOOSTAsyncSocket> pxBOOSTAsyncNewPeerSocket(new XBOOSTAsyncSocket(m_pIOContext));
		boost::shared_ptr<XBOOSTAsyncSocket> pxBOOSTAsyncNewPeerSocket =
			boost::make_shared<XBOOSTAsyncSocket>(m_pIOContext);
		m_pAcceptor->async_accept(*(*pxBOOSTAsyncNewPeerSocket)(), *GetEndpoint(nPort),
			boost::bind(&XBOOSTAsyncServer::AcceptHandler, this, boost::asio::placeholders::error,
				pxBOOSTAsyncNewPeerSocket, nPort));

		pXPeerSocket->Read();
	}

	boost::shared_ptr<boost::asio::io_context>& operator ()()
	{
		return m_pIOContext;
	}
private:
	boost::shared_ptr<boost::asio::io_context> m_pIOContext;
	//boost::shared_ptr<boost::asio::io_context::work> m_pWork;
	boost::shared_ptr<boost::thread_group> m_pThreadGroup;

	boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_pAcceptor;
};

void _tmain()
{
	XBOOSTAsyncServer xBOOSTAsyncServer(5800);
	xBOOSTAsyncServer.run();
	xBOOSTAsyncServer.Accept(5800);
	//XBOOSTAsyncSocket xBOOSTAsyncSocket(xBOOSTAsyncServer());
	//xBOOSTAsyncSocket.Connect("127.0.0.1", 16365);
	std::cin.get();
}*/

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <winerror.h>

#include <iostream>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

class XBOOSTAsyncProxySocket : public boost::enable_shared_from_this<XBOOSTAsyncProxySocket>
{
public:
	XBOOSTAsyncProxySocket(boost::shared_ptr<boost::asio::io_context>& pIOContext)
		: m_pSocket(new boost::asio::ip::tcp::socket(*pIOContext)),
		m_bServer(false)
	{
		std::cout << ((m_bServer) ? "Server " : "Client ") << "XBOOSTAsyncProxySocket" << std::endl;
		m_arrBuffer.resize(1024);
	}

	virtual ~XBOOSTAsyncProxySocket()
	{
		std::cout << ((m_bServer) ? "Server " : "Client ") << "~XBOOSTAsyncProxySocket" << std::endl;
		Close();
		std::cout << ((m_bServer) ? "Server " : "Client ") << "~XBOOSTAsyncProxySocket reset" << std::endl;
		m_pxBOOSTAsyncForwardSocket.reset();
	}

	void Close()
	{
		std::cout << ((m_bServer) ? "Server " : "Client ") << "Close" << std::endl;
		boost::system::error_code ec;
		if (m_pSocket->is_open())
		{
			m_pSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			m_pSocket->close(ec);
		}
		std::cout << ((m_bServer) ? "Server " : "Client ") << "Close end" << std::endl;
	}

	boost::asio::ip::tcp::socket& GetSocket()
	{
		return *m_pSocket;
	}

	boost::shared_ptr<boost::asio::ip::tcp::socket>& operator ()()
	{
		return m_pSocket;
	}

	boost::shared_ptr<boost::asio::ip::tcp::endpoint> GetEndpoint(std::string szHost, int nPort)
	{
		boost::shared_ptr<boost::asio::ip::tcp::resolver>
			pResolver(new boost::asio::ip::tcp::resolver(m_pSocket->get_executor()));
		boost::shared_ptr<boost::asio::ip::tcp::resolver::query>
			pQuery(new boost::asio::ip::tcp::resolver::query(szHost, boost::lexical_cast<std::string>(nPort)));
		boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint =
			boost::make_shared<boost::asio::ip::tcp::endpoint>(*pResolver->resolve(*pQuery));

		return pEndpoint;
	}

	void Connect(std::string szHost, int nPort)
	{
		std::cout << "Client: " << shared_from_this().use_count() << std::endl;
		m_pSocket->async_connect(*GetEndpoint(szHost, nPort),
			boost::bind(&XBOOSTAsyncProxySocket::ConnectHandler, shared_from_this(), boost::asio::placeholders::error));
		std::cout << "Client: " << shared_from_this().use_count() << std::endl;
	}

	void ConnectHandler(const boost::system::error_code& ec)
	{
		std::cout << "Client " << "ConnectHandler:" << std::endl;
		std::cout << "Client " << shared_from_this().use_count() << std::endl;
		if (ec == boost::system::errc::success)
		{
			m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
				boost::bind(&XBOOSTAsyncProxySocket::read_handler, shared_from_this(), boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
			std::cout << ((m_bServer) ? "Server " : "Client ") << ec << std::endl;
		std::cout << "Client: " << shared_from_this().use_count() << std::endl;
	}

	void Read()
	{
		std::cout << ((m_bServer)? "Server " : "Client ") << "Read" << std::endl;
		std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;

		m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
			boost::bind(&XBOOSTAsyncProxySocket::read_handler, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	}

	void Write(std::vector<BYTE>& pBuffer, std::size_t bytes_transferred)
	{
		if (bytes_transferred != 0)
		{
			std::cout << ((m_bServer) ? "Server " : "Client ") << "Write " << bytes_transferred << std::endl;
			std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
			std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
			m_pSocket->async_write_some(boost::asio::buffer(pBuffer.data(), bytes_transferred),
				boost::bind(&XBOOSTAsyncProxySocket::write_handler, shared_from_this(), boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
			std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
		}
	}

	void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		std::cout << ((m_bServer) ? "Server " : "Client ") << "read_handler: " << bytes_transferred << std::endl;
		std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
		if (ec == boost::system::errc::success)
		{
			// Do Write
			if (bytes_transferred != 0)
			{
				if (m_pxBOOSTAsyncForwardSocket)
				{
					m_pxBOOSTAsyncForwardSocket->Write(m_arrBuffer, bytes_transferred);
					m_pxBOOSTAsyncForwardSocket->m_bytes_transferred = bytes_transferred;
				}
			}
			else
				Read();
			//std::cout << m_arrBuffer[0] << std::endl;

			std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
			std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
		}
		else
		{
			std::cout << ((m_bServer) ? "Server " : "Client ") << ec << std::endl;
			if (m_pxBOOSTAsyncForwardSocket)
			{
				//m_pxBOOSTAsyncForwardSocket->ResetForwardSocket(); // Need to decide whether its required ir not. Or close the socket.
				// Or
				m_pxBOOSTAsyncForwardSocket->Close();
				m_pxBOOSTAsyncForwardSocket.reset();
			}
			std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		}
	}

	void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		std::cout << ((m_bServer) ? "Server " : "Client ") << "write_handler: " << bytes_transferred << std::endl;
		std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
		if (ec == boost::system::errc::success)
		{
			// Initiate the read call.
			if (bytes_transferred != 0)
			{
				if (m_pxBOOSTAsyncForwardSocket)
				{
					m_pxBOOSTAsyncForwardSocket->Read();
				}
			}
			else
				Write(m_arrBuffer, m_bytes_transferred);
			std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
			std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
		}
		else
		{
			std::cout << ((m_bServer) ? "Server " : "Client ") << ec << std::endl;
			if (m_pxBOOSTAsyncForwardSocket)
			{
				//m_pxBOOSTAsyncForwardSocket->ResetForwardSocket(); // Need to decide whether its required ir not. Or close the socket.
				// Or
				m_pxBOOSTAsyncForwardSocket->Close(); // Need to be careful here, it might crash. Qick fix place a sleep after this call.
				std::cout << ((m_bServer) ? "Server " : "Client ") << "reset" << std::endl;
				m_pxBOOSTAsyncForwardSocket.reset();
			}
			std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		}
	}

	void SetForwardSocket(boost::shared_ptr<XBOOSTAsyncProxySocket>& pXBOOSTAsyncProxySocket)
	{
		m_pxBOOSTAsyncForwardSocket = pXBOOSTAsyncProxySocket;
	}

	void ResetForwardSocket()
	{
		m_pxBOOSTAsyncForwardSocket.reset();
	}

	void SetServer()
	{
		m_bServer = true;
	}

	bool m_bServer;
	std::size_t m_bytes_transferred;
private:
	boost::shared_ptr<boost::asio::ip::tcp::socket> m_pSocket;
	boost::shared_ptr<XBOOSTAsyncProxySocket> m_pxBOOSTAsyncForwardSocket;
	std::vector<BYTE> m_arrBuffer;
};

class XBOOSTAsyncProxyServer
{
public:
	XBOOSTAsyncProxyServer(int nPort) : m_pIOContext(new boost::asio::io_context()),
		m_pWork(new boost::asio::io_context::work(*m_pIOContext)),
		m_pThreadGroup(new boost::thread_group())
	{
		std::cout << "XBOOSTAsyncProxyServer" << std::endl;
		m_pEndpoint1 = GetEndpoint(nPort);
		m_pAcceptor = boost::make_shared<boost::asio::ip::tcp::acceptor>(*m_pIOContext, *m_pEndpoint1);
		//m_pAcceptor(new boost::asio::ip::tcp::acceptor(*m_pIOContext))
	}

	virtual ~XBOOSTAsyncProxyServer()
	{
		std::cout << "~XBOOSTAsyncProxyServer" << std::endl;
		Stop();
		//m_pIOContext->
	}

	void Run()
	{
		std::cout << "Run" << std::endl;
		m_pIOContext->restart();
		m_pIOContext->run();
		std::cout << "Run end" << std::endl;
	}

	void Start()
	{
		std::cout << "Start" << std::endl;
		m_pThreadGroup->create_thread(boost::bind(&XBOOSTAsyncProxyServer::Run, this));
		//m_pThreadGroup->create_thread(boost::bind(&boost::asio::io_context::run, *m_pIOContext));
		std::cout << "Start end" << std::endl;
	}

	void Stop()
	{
		std::cout << "Stop" << std::endl;
		m_pIOContext->stop();
		std::cout << "Stop end" << std::endl;
	}

	boost::shared_ptr<boost::asio::ip::tcp::endpoint> GetEndpoint(int nPort)
	{
		boost::shared_ptr<boost::asio::ip::tcp::resolver>
			pResolver(new boost::asio::ip::tcp::resolver(*m_pIOContext));
		boost::shared_ptr<boost::asio::ip::tcp::resolver::query>
			pQuery(new boost::asio::ip::tcp::resolver::query(boost::asio::ip::tcp::v4(),
				boost::lexical_cast<std::string>(nPort)));
		boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint =
			boost::make_shared<boost::asio::ip::tcp::endpoint>(*pResolver->resolve(*pQuery));

		//boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint1 =
		//	boost::make_shared<boost::asio::ip::tcp::endpoint>(
		//		boost::asio::ip::address_v4::any(), nPort);

		return pEndpoint;
	}

	void Accept(int nPort)
	{
		std::cout << "Accept New" << std::endl;
		//XBOOSTAsyncSocket xBOOSTAsyncPeerSocket(m_pIOContext);
		//boost::shared_ptr<XBOOSTAsyncSocket> pxBOOSTAsyncPeerSocket(new XBOOSTAsyncSocket(m_pIOContext));
		//m_pEndpoint1 = GetEndpoint(nPort);

		//m_pAcceptor->open(m_pEndpoint1->protocol());
		//m_pAcceptor->bind(*m_pEndpoint1);
		//m_pAcceptor->listen();
		//m_pAcceptor->async_accept(*m_pSocket, *m_pEndpoint1,
		//	boost::bind(&XBOOSTAsyncServer::AcceptHandler, this, boost::asio::placeholders::error,
		//		pxBOOSTAsyncPeerSocket, nPort));
		//m_pAcceptor->async_accept(pxBOOSTAsyncPeerSocket->GetSocket(), *m_pEndpoint1,
		//	boost::bind(&XBOOSTAsyncProxyServer::AcceptHandler, this, boost::asio::placeholders::error,
		//		pxBOOSTAsyncPeerSocket, nPort));
		boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncPeerSocket =
			boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext);
		m_pAcceptor->async_accept(pxBOOSTAsyncPeerSocket->GetSocket(),
			boost::bind(&XBOOSTAsyncProxyServer::AcceptHandler, this, boost::asio::placeholders::error,
				pxBOOSTAsyncPeerSocket, nPort));

		std::cout << "Peer: " << pxBOOSTAsyncPeerSocket.use_count() << std::endl;
		//m_pAcceptor->async_accept(*(*pxBOOSTAsyncPeerSocket)(), *GetEndpoint(nPort),
		//	boost::bind(&XBOOSTAsyncServer::AcceptHandler, this, boost::asio::placeholders::error,
		//		pxBOOSTAsyncPeerSocket, nPort));
		//return pxBOOSTAsyncPeerSocket;
	}

	void AcceptHandler(const boost::system::error_code& ec, boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket, int nPort)
	{
		std::cout << "AcceptHandler" << std::endl;

		std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;

		//boost::shared_ptr<XBOOSTAsyncSocket> pxBOOSTAsyncNewPeerSocket(new XBOOSTAsyncSocket(m_pIOContext));
		boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncNewPeerSocket =
			boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext);
		m_pAcceptor->async_accept(pxBOOSTAsyncNewPeerSocket->GetSocket(), *GetEndpoint(nPort),
			boost::bind(&XBOOSTAsyncProxyServer::AcceptHandler, this, boost::asio::placeholders::error,
				pxBOOSTAsyncNewPeerSocket, nPort));

		if (ec == boost::system::errc::success)
		{
			// Create client
			std::cout << "Connect to PW server" << std::endl;
			boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncNewClientSocket =
				boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext);
			std::cout << "Client before connect: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;
			pxBOOSTAsyncNewClientSocket->Connect("127.0.0.1", 16365);
			Sleep(2000);
			std::cout << "Client after connect: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;

			std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;
			std::cout << "Client: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;

			pXPeerSocket->SetServer();
			pXPeerSocket->SetForwardSocket(pxBOOSTAsyncNewClientSocket);
			pxBOOSTAsyncNewClientSocket->SetForwardSocket(pXPeerSocket);

			std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;
			std::cout << "Client: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;
			pXPeerSocket->Read();

			std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;
			std::cout << "Client: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;
			std::cout << "AcceptHandler End" << std::endl;
		}
		/*boost::shared_ptr<boost::asio::io_context> pIOContext1(new boost::asio::io_context());
		boost::thread_group tg;

		boost::shared_ptr<boost::asio::ip::tcp::endpoint>
			endpoint1(new boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5800));
		boost::shared_ptr<boost::asio::ip::tcp::acceptor>
			paccept1(new boost::asio::ip::tcp::acceptor(*pIOContext1, *endpoint1));
		//boost::asio::ip::tcp::socket socket1(iocontext1);
		boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket1;
		pSocket1 = boost::make_shared<boost::asio::ip::tcp::socket>(*pIOContext1);

		boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncNewPeerSocket =
			boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext);
		paccept1->async_accept(*pSocket1,
			boost::bind(&XBOOSTAsyncProxyServer::AcceptHandler, this,
				boost::asio::placeholders::error, pxBOOSTAsyncNewPeerSocket, nPort));*/

	}

	boost::shared_ptr<boost::asio::io_context>& operator ()()
	{
		return m_pIOContext;
	}

	void JoinAll()
	{
		m_pThreadGroup->join_all();
	}
private:
	boost::shared_ptr<boost::asio::io_context> m_pIOContext;
	boost::shared_ptr<boost::asio::io_context::work> m_pWork;
	boost::shared_ptr<boost::thread_group> m_pThreadGroup;

	boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_pAcceptor;
	boost::shared_ptr<boost::asio::ip::tcp::endpoint> m_pEndpoint1;
};

void _tmain()
{
	XBOOSTAsyncProxyServer xBOOSTAsyncServer(5800);
	xBOOSTAsyncServer.Start();
	xBOOSTAsyncServer.Accept(5800);
	//XBOOSTAsyncSocket xBOOSTAsyncSocket(xBOOSTAsyncServer());
	//xBOOSTAsyncSocket.Connect("127.0.0.1", 16365);
	std::cin.get();
	std::cout << "Stop" << std::endl;
	xBOOSTAsyncServer.Stop();
	std::cout << "JoinAll" << std::endl;
	xBOOSTAsyncServer.JoinAll();

	std::cout << "_tmain end" << std::endl;
}
