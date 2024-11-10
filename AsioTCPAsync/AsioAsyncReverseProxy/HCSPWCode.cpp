#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <winerror.h>

#include <iostream>
#include <string>
using namespace std;

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#define _CRT_SECURE_NO_WARNINGS

enum XBufferState
{
	XBufferStateNoFileCapture = -1,
	XBufferStateFileCaptureFinished = 1,
	XBufferStateReadFileCaptureContinue = 0,
	XBufferStateWriteFileCaptureContinue = 2
};

class MessageRequestHandler {};

class XBOOSTMutex
{
public:
	XBOOSTMutex();
	~XBOOSTMutex();
private:
	boost::mutex m_Mutex;
};

class XBOOSTAsyncProxySocket
	: public boost::enable_shared_from_this<XBOOSTAsyncProxySocket>
{
public:
	XBOOSTAsyncProxySocket(boost::shared_ptr<boost::asio::io_context>& pIOContext,
		const std::wstring& wszServiceName);
	virtual ~XBOOSTAsyncProxySocket();
	void Close();
	boost::asio::ip::tcp::socket& GetSocket();
	boost::shared_ptr<boost::asio::ip::tcp::socket>& operator ()();

	boost::shared_ptr<boost::asio::ip::tcp::endpoint> GetEndpoint(const std::wstring& wszHost, int nPort);

	void Connect(const std::wstring& wszHost, int nPort, boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket);
	void ConnectHandler(const boost::system::error_code& ec, boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket);

	bool Read();
	bool Write(std::vector<BYTE>& pBuffer, std::size_t bytes_transferred,
		boost::shared_ptr<MessageRequestHandler> pMessageRequestHandler = nullptr);

	void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred);

	void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred,
		boost::shared_ptr<MessageRequestHandler> pMessageRequestHandler);

	int HandleProxyServer(const int& nProxyServerProxyClient, std::vector<BYTE>& pBuffer,
		std::size_t& bytes_transferred);

	//int HandleProxyClient(std::vector<BYTE>& pBuffer, std::size_t& bytes_transferred);

	void SetForwardSocket(boost::shared_ptr<XBOOSTAsyncProxySocket>& pXBOOSTAsyncProxySocket);

	boost::shared_ptr<XBOOSTAsyncProxySocket> GetForwardSocket();

	void ResetForwardSocket();

	void SetServer();

	bool IsServer() { return m_bServer; }

	boost::shared_ptr<MessageRequestHandler> GetMessageHanler()
	{
		return m_pMessageRequestHandler;
	}
	std::vector<BYTE>& GetBuffer() { return m_arrBuffer; }
	std::size_t GetBytesTransfered() { return m_bytes_transferred; }
	void SetBytesTransfered(std::size_t byte_trnferred) { m_bytes_transferred = byte_trnferred; }
	void SetEndpoint(boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint);
private:

	//
	// The following functions are dubug strings to print in DebugView.
	//
	void OutputDbgStr(const std::string& str);
	void OutputDbgStr(const long& lNum);
	void OutputDbgStr(bool bServer, const long& lNum);
	void OutputDbgStr(const std::string& str, const long& lNum);

	bool m_bServer;
	std::vector<BYTE> m_arrBuffer;
	std::size_t m_bytes_transferred;

	boost::shared_ptr<boost::asio::ip::tcp::socket> m_pSocket;
	boost::shared_ptr<boost::asio::ip::tcp::endpoint> m_pEndpoint;
	boost::shared_ptr<XBOOSTAsyncProxySocket> m_pxBOOSTAsyncForwardSocket;

	std::wstring m_wszServiceName;
	boost::shared_ptr<MessageRequestHandler> m_pMessageRequestHandler;
};

class XBOOSTAsyncProxyAcceptor
{
public:
	XBOOSTAsyncProxyAcceptor();
	XBOOSTAsyncProxyAcceptor(int nReverseProxyPort, const std::wstring& szServerHost, int nServerPort);

	virtual ~XBOOSTAsyncProxyAcceptor();

	virtual void Init(int nProxyServerPort, const std::wstring& szServerHost,
		int nServerPort, const std::wstring& wszServiceName);
	virtual void InitThreadPool();

	void InitLogPurgerThread();
	void LogPurger();

	void Run();

	void Start();

	void Stop();

	boost::shared_ptr<boost::asio::ip::tcp::endpoint> GetEndpoint(int nPort);

	void Accept();

	void AcceptHandler(const boost::system::error_code& ec, boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket);

	void AcceptorClose();

	boost::shared_ptr<boost::asio::io_context>& operator ()();
	void JoinAll();
private:

	//
	// The following functions are dubug strings to print in DebugView.
	//
	void OutputDbgStr(const std::string& str);
	void OutputDbgStr(const long& lNum);
	void OutputDbgStr(bool bServer, const long& lNum);
	void OutputDbgStr(const std::string& str, const long& lNum);

	boost::shared_ptr<boost::asio::io_context> m_pIOContext;
	boost::shared_ptr<boost::asio::io_context::work> m_pWork;
	boost::shared_ptr<boost::thread_group> m_pThreadGroup;

	boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_pAcceptor;
	boost::shared_ptr<boost::asio::ip::tcp::endpoint> m_pEndpoint1;

	std::wstring m_szServerHost;
	int m_nServerPort;
	int m_nReverseProxyPort;
	std::wstring m_wszServiceName;
};

XBOOSTMutex::XBOOSTMutex()
{
	m_Mutex.lock();
}

XBOOSTMutex::~XBOOSTMutex()
{
	m_Mutex.unlock();
}

void XBOOSTAsyncProxySocket::OutputDbgStr(const string& str)
{
	string wszStr;
	wszStr = ((m_bServer) ? "Server " : "Client ");
	wszStr += str.c_str();
	//OutputDebugStringA(wszStr.c_str());
	cout << wszStr.c_str() << endl;
}

void XBOOSTAsyncProxySocket::OutputDbgStr(const long& lNum)
{
	string wszStr;
	wszStr = ((m_bServer) ? "Server " : "Client ");
	wszStr += to_string(lNum);
	//OutputDebugStringA(wszStr.c_str());
	cout << wszStr.c_str() << endl;
}

void XBOOSTAsyncProxySocket::OutputDbgStr(bool bServer, const long& lNum)
{
	string wszStr;
	wszStr = ((bServer) ? "Server " : "Client ");
	wszStr += to_string(lNum);
	//OutputDebugStringA(wszStr.c_str());
	cout << wszStr.c_str() << endl;
}

void XBOOSTAsyncProxySocket::OutputDbgStr(const string & str, const long& lNum)
{
	string wszStr;
	wszStr = str.c_str();
	wszStr += to_string(lNum);
	//OutputDebugStringA(wszStr.c_str());
	cout << wszStr.c_str() << endl;
}

XBOOSTAsyncProxySocket::XBOOSTAsyncProxySocket(boost::shared_ptr<boost::asio::io_context>& pIOContext,
	const wstring& wszServiceName)
	: m_pSocket(new boost::asio::ip::tcp::socket(*pIOContext)),
	m_bServer(false),
	m_wszServiceName(wszServiceName)
{
	OutputDbgStr("XBOOSTAsyncProxySocket");
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "XBOOSTAsyncProxySocket" << std::endl;
	m_arrBuffer.resize(1024);

	//m_pMessageRequestHandler = MessageRequestHandlerFactory::getRequestHandler(0, m_wszServiceName);
}

XBOOSTAsyncProxySocket::~XBOOSTAsyncProxySocket()
{
	OutputDbgStr("~XBOOSTAsyncProxySocket");
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "~XBOOSTAsyncProxySocket" << std::endl;
	Close();
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "~XBOOSTAsyncProxySocket reset" << std::endl;
	OutputDbgStr("~XBOOSTAsyncProxySocket reset");
	m_pxBOOSTAsyncForwardSocket.reset();
}

void XBOOSTAsyncProxySocket::Close()
{
	OutputDbgStr("Close");
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "Close" << std::endl;
	boost::system::error_code ec;
	if (m_pSocket->is_open())
	{
		m_pSocket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		m_pSocket->close(ec);
	}
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "Close end" << std::endl;
	OutputDbgStr("Close end");
}

boost::asio::ip::tcp::socket& XBOOSTAsyncProxySocket::GetSocket()
{
	return *m_pSocket;
}

boost::shared_ptr<boost::asio::ip::tcp::socket>& XBOOSTAsyncProxySocket::operator ()()
{
	return m_pSocket;
}

boost::shared_ptr<boost::asio::ip::tcp::endpoint> XBOOSTAsyncProxySocket::GetEndpoint(const wstring& wszHost, int nPort)
{
	boost::shared_ptr<boost::asio::ip::tcp::resolver>
		pResolver(new boost::asio::ip::tcp::resolver(m_pSocket->get_executor()));
	string szHost(wszHost.begin(), wszHost.end());
	boost::shared_ptr<boost::asio::ip::tcp::resolver::query>
		pQuery(new boost::asio::ip::tcp::resolver::query(szHost.c_str(), boost::lexical_cast<std::string>(nPort)));
	boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint =
		boost::make_shared<boost::asio::ip::tcp::endpoint>(*pResolver->resolve(*pQuery));

	return pEndpoint;
}

void XBOOSTAsyncProxySocket::SetEndpoint(boost::shared_ptr<boost::asio::ip::tcp::endpoint> pEndpoint)
{
	m_pEndpoint = pEndpoint;
}

void XBOOSTAsyncProxySocket::Connect(const wstring& wszHost, int nPort,
	boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket)
{
	cout << "Host" << wszHost.c_str() << endl;
	OutputDbgStr(shared_from_this().use_count());
	//std::cout << "Client: " << shared_from_this().use_count() << std::endl;
	m_pEndpoint = GetEndpoint(wszHost, nPort);
	m_pSocket->async_connect(*m_pEndpoint,
		boost::bind(&XBOOSTAsyncProxySocket::ConnectHandler, shared_from_this(),
			boost::asio::placeholders::error, pXPeerSocket));
	//m_pSocket->async_connect(*GetEndpoint(wszHost, nPort),
	//	boost::bind(&XBOOSTAsyncProxySocket::ConnectHandler, shared_from_this(),
	//		boost::asio::placeholders::error, pXPeerSocket));
	//std::cout << "Client: " << shared_from_this().use_count() << std::endl;
	OutputDbgStr(shared_from_this().use_count());
}

void XBOOSTAsyncProxySocket::ConnectHandler(const boost::system::error_code& ec,
	boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket)
{
	OutputDbgStr("ConnectHandler:");
	//std::cout << "Client " << "ConnectHandler:" << std::endl;
	OutputDbgStr(shared_from_this().use_count());
	//std::cout << "Client " << shared_from_this().use_count() << std::endl;
	if (ec == boost::system::errc::success)
	{
		OutputDbgStr("Peer: ", pXPeerSocket.use_count());
		//std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;
		OutputDbgStr("Client: ", shared_from_this().use_count());
		//std::cout << "Client: " << shared_from_this().use_count() << std::endl;

		boost::shared_ptr<XBOOSTAsyncProxySocket> pSelf = shared_from_this();
		pXPeerSocket->SetForwardSocket(pSelf);
		SetForwardSocket(pXPeerSocket);

		OutputDbgStr("Peer: ", pXPeerSocket.use_count());
		//std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;
		OutputDbgStr("Client: ", shared_from_this().use_count());
		//std::cout << "Client: " << shared_from_this().use_count() << std::endl;
		pXPeerSocket->Read();

		OutputDbgStr("Peer: ", pXPeerSocket.use_count());
		//std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;
		OutputDbgStr("Client: ", shared_from_this().use_count());
		//std::cout << "Client: " << shared_from_this().use_count() << std::endl;

		m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
			boost::bind(&XBOOSTAsyncProxySocket::read_handler, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		OutputDbgStr("Error: ");
		OutputDbgStr(ec.value());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << ec << std::endl;
	}
	OutputDbgStr("Client: ", shared_from_this().use_count());
	//std::cout << "Client: " << shared_from_this().use_count() << std::endl;
}

bool XBOOSTAsyncProxySocket::Read()
{
	bool bRet = false;
	boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncForwardSocket = GetForwardSocket();
	OutputDbgStr("Read");
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "Read" << std::endl;
	OutputDbgStr(shared_from_this().use_count());
	//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
	if (pxBOOSTAsyncForwardSocket)
	{
		OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
	}
	//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	if (m_pSocket->is_open())
	{
		m_pSocket->async_read_some(boost::asio::buffer(m_arrBuffer),
			boost::bind(&XBOOSTAsyncProxySocket::read_handler, shared_from_this(), boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		bRet = true;
	}
	else if (pxBOOSTAsyncForwardSocket)
	{
		pxBOOSTAsyncForwardSocket->ResetForwardSocket();
	}
	OutputDbgStr(shared_from_this().use_count());
	//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
	if (pxBOOSTAsyncForwardSocket)
		OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
	//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	return bRet;
}

bool XBOOSTAsyncProxySocket::Write(std::vector<BYTE>& pBuffer, std::size_t bytes_transferred,
	boost::shared_ptr<MessageRequestHandler> pMessageRequestHandler)
{
	bool bRet = false;
	boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncForwardSocket = GetForwardSocket();
	if (bytes_transferred != 0)
	{
		OutputDbgStr("Write");
		OutputDbgStr(bytes_transferred);
		//std::cout << ((m_bServer) ? "Server " : "Client ") << "Write " << bytes_transferred << std::endl;
		OutputDbgStr(shared_from_this().use_count());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		if (pxBOOSTAsyncForwardSocket)
			OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
		//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") <<
		//m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
		if (m_pSocket->is_open())
		{
			m_pSocket->async_write_some(boost::asio::buffer(pBuffer.data(), bytes_transferred),
				boost::bind(&XBOOSTAsyncProxySocket::write_handler, shared_from_this(), boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred, pMessageRequestHandler));
			bRet = true;
		}
		else if (pxBOOSTAsyncForwardSocket)
		{
			pxBOOSTAsyncForwardSocket->ResetForwardSocket();
		}
		OutputDbgStr(shared_from_this().use_count());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		if (pxBOOSTAsyncForwardSocket)
			OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
		//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	}
	else if (pxBOOSTAsyncForwardSocket)
	{
		pxBOOSTAsyncForwardSocket->ResetForwardSocket();
	}
	return bRet;
}

void XBOOSTAsyncProxySocket::read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
	//if (ec)
	//{
	//	OutputDbgStr("*********************read_handler error*******************", ec.value());
	//}
	bool bDataExist = false;
	bDataExist = (m_arrBuffer[0] == 0x42);
	bDataExist &= (m_arrBuffer[1] == 0x01 || m_arrBuffer[1] == 0x00);
	//if (bDataExist && bytes_transferred > 60)
	if (bDataExist && bytes_transferred == 5)
	{
		bDataExist = false;
		Sleep(120000); // Two minutes
	}
	//SetBytesTransfered(bytes_transferred);
	//m_pxBOOSTAsyncForwardSocket->Write(m_arrBuffer, bytes_transferred);
	//return;
	boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncForwardSocket = GetForwardSocket();
	OutputDbgStr("read_handler");
	OutputDbgStr(bytes_transferred);
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "read_handler: " << bytes_transferred << std::endl;
	OutputDbgStr(shared_from_this().use_count());
	//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
	if (pxBOOSTAsyncForwardSocket)
		OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
	//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	if (ec == boost::system::errc::success)
	{
		// Do Write
		if (bytes_transferred != 0)
		{
			if (pxBOOSTAsyncForwardSocket)
			{
				// Call the ProceBuffer
				//boost::shared_ptr<MessageRequestHandler> pMessageRequestHandler;

				int nRet = XBufferState::XBufferStateNoFileCapture;
				nRet = HandleProxyServer(m_bServer, m_arrBuffer, bytes_transferred);
				if (nRet == XBufferState::XBufferStateNoFileCapture || nRet == XBufferState::XBufferStateFileCaptureFinished)
				{
					SetBytesTransfered(bytes_transferred);
					pxBOOSTAsyncForwardSocket->Write(m_arrBuffer, bytes_transferred);
				}
				else if (nRet == XBufferState::XBufferStateWriteFileCaptureContinue)
				{
					SetBytesTransfered(bytes_transferred);
					pxBOOSTAsyncForwardSocket->Write(m_arrBuffer, bytes_transferred, m_pMessageRequestHandler);
				}
				else if (nRet == XBufferState::XBufferStateReadFileCaptureContinue)
				{
					Read();
				}
			}
		}
		else
			Read();
		//std::cout << m_arrBuffer[0] << std::endl;

		OutputDbgStr(shared_from_this().use_count());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		if (pxBOOSTAsyncForwardSocket)
			OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
		//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") << m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	}
	else
	{
		OutputDbgStr(ec.value());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << ec << std::endl;
		if (pxBOOSTAsyncForwardSocket)
			pxBOOSTAsyncForwardSocket->ResetForwardSocket(); // Need to decide whether its required ir not. Or close the socket.
		// Or
		//m_pxBOOSTAsyncForwardSocket->Close();
		//m_pxBOOSTAsyncForwardSocket.reset();
		//m_pxBOOSTAsyncForwardSocket = nullptr;
		OutputDbgStr(shared_from_this().use_count());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
	}
}

void XBOOSTAsyncProxySocket::write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred,
	boost::shared_ptr<MessageRequestHandler> pMessageRequestHandler)
{
	//m_pxBOOSTAsyncForwardSocket->Read();
	//return;

	boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncForwardSocket = GetForwardSocket();
	OutputDbgStr("write_handler: ");
	OutputDbgStr(bytes_transferred);
	//std::cout << ((m_bServer) ? "Server " : "Client ") << "write_handler: " << bytes_transferred << std::endl;
	OutputDbgStr(shared_from_this().use_count());
	//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
	if (pxBOOSTAsyncForwardSocket)
		OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
	//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") <<
	//	m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	if (ec == boost::system::errc::success)
	{
		// Initiate the read call.
		if (pxBOOSTAsyncForwardSocket)
		{
			if (bytes_transferred != 0)
			{
				if (pMessageRequestHandler)
				{
					bytes_transferred = 0; // Tell the Halder that its for write call and get the buffer.
					int nRet = pxBOOSTAsyncForwardSocket->HandleProxyServer(pxBOOSTAsyncForwardSocket->IsServer(),
						pxBOOSTAsyncForwardSocket->GetBuffer(),
						bytes_transferred);
					if (nRet == XBufferState::XBufferStateNoFileCapture || nRet == XBufferState::XBufferStateFileCaptureFinished)
					{
						pxBOOSTAsyncForwardSocket->SetBytesTransfered(bytes_transferred);
						Write(pxBOOSTAsyncForwardSocket->GetBuffer(), bytes_transferred);
					}
					else if (nRet == XBufferState::XBufferStateWriteFileCaptureContinue)
					{
						pxBOOSTAsyncForwardSocket->SetBytesTransfered(bytes_transferred);
						Write(pxBOOSTAsyncForwardSocket->GetBuffer(), bytes_transferred, m_pMessageRequestHandler);
					}
				}
				else
				{
					pxBOOSTAsyncForwardSocket->Read();
				}
			}
			else
				Write(pxBOOSTAsyncForwardSocket->GetBuffer(), pxBOOSTAsyncForwardSocket->GetBytesTransfered(),
					pMessageRequestHandler);
		}
		OutputDbgStr(shared_from_this().use_count());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
		if (pxBOOSTAsyncForwardSocket)
			OutputDbgStr(pxBOOSTAsyncForwardSocket->m_bServer, pxBOOSTAsyncForwardSocket.use_count());
		//std::cout << ((m_pxBOOSTAsyncForwardSocket->m_bServer) ? "Server " : "Client ") <<
		//	m_pxBOOSTAsyncForwardSocket.use_count() << std::endl;
	}
	else
	{
		OutputDbgStr(ec.value());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << ec << std::endl;
		if (pxBOOSTAsyncForwardSocket)
			pxBOOSTAsyncForwardSocket->ResetForwardSocket(); // Need to decide whether its required ir not. Or close the socket.
		// Or
		//m_pxBOOSTAsyncForwardSocket->Close(); // Need to be careful here, it might crash. Qick fix place a sleep after this call.
		//OutputDbgStr("reset");
		//std::cout << ((m_bServer) ? "Server " : "Client ") << "reset" << std::endl;
		//m_pxBOOSTAsyncForwardSocket.reset();
		//m_pxBOOSTAsyncForwardSocket = nullptr;
		OutputDbgStr(shared_from_this().use_count());
		//std::cout << ((m_bServer) ? "Server " : "Client ") << shared_from_this().use_count() << std::endl;
	}
}

int XBOOSTAsyncProxySocket::HandleProxyServer(const int& nProxyServerProxyClient, std::vector<BYTE>& pBuffer,
	std::size_t& bytes_transferred)
{
	int nRet = XBufferState::XBufferStateNoFileCapture;
	bool bDataExist = false;
	bDataExist = (pBuffer[0] == 0x42);
	bDataExist &= (pBuffer[1] == 0x01 || pBuffer[1] == 0x00);
	//if (bDataExist && bytes_transferred > 60)
	if (bDataExist && bytes_transferred == 5)
	{
		bDataExist = false;
		Sleep(120000); // Two minutes
	}

	try
	{
		do
		{
			if (!m_pMessageRequestHandler)
			{
				// XBoostCommonUtil::Log(szRequestType + L" Not Implemented");
				// pMessageRequestHandler = nullptr;
				break;
			}

			{
				////XBOOSTMutex xBOOSTMutex;
				//nRet = m_pMessageRequestHandler->HandleRequest(nProxyServerProxyClient, m_arrBuffer, bytes_transferred,
				//	m_pxBOOSTAsyncForwardSocket->GetMessageHanler());
			}
			if ((nRet == XBufferState::XBufferStateNoFileCapture) || (nRet == XBufferState::XBufferStateFileCaptureFinished))
			{
				//pMessageRequestHandler = nullptr; // Dont delete. Delete only when seddion ends.
				break;
			}
		} while (FALSE);
	}
	//catch (XException& ex)
	//{
	//	nRet = XBufferState::XBufferStateNoFileCapture;
	//}
	catch (boost::system::system_error& e)
	{
		nRet = XBufferState::XBufferStateNoFileCapture;
	}
	catch (std::exception& /*ex*/)
	{
		nRet = XBufferState::XBufferStateNoFileCapture;
		//std::cout << ex.what() << std::endl;
	}

	return nRet;
}

//int XBOOSTAsyncProxySocket::HandleProxyClient(std::vector<BYTE>& pBuffer, std::size_t& bytes_transferred)
//{
//	XConfigServer& xConfigServer = XConfigServer::GetInstance();
//	XBoostCommonUtil::Log(L"HandleProxyServer function  called.\n");
//
//	XStream<WCHAR> wszKeyGlobalIP;
//	//wszKeyGlobalIP = xConfigServer.GetProperty(CONF_KEY_GLOBALREQUEST_NAME);
//
//
//	try
//	{
//		//m_io_service->post(boost::bind(&XBoostServer::HandleClients, this, clientsocket));
//	}
//	catch (XException& ex)
//	{
//		XLog(xConfigServer).Print(log_sys, log_info, ex.GetMessageText());
//	}
//	catch (boost::system::system_error& e)
//	{
//		std::string szErrorMessage = e.what();
//		std::wstring szOutputMsg = L"";
//		XBoostCommonUtil::ConvertToWString(szErrorMessage, szOutputMsg);
//		XLog(xConfigServer).Print(log_sys, log_info, szOutputMsg.c_str());
//	}
//
//	return 0;
//}

void XBOOSTAsyncProxySocket::SetForwardSocket(boost::shared_ptr<XBOOSTAsyncProxySocket>& pXBOOSTAsyncProxySocket)
{
	m_pxBOOSTAsyncForwardSocket = pXBOOSTAsyncProxySocket;
}

boost::shared_ptr<XBOOSTAsyncProxySocket> XBOOSTAsyncProxySocket::GetForwardSocket()
{
	return m_pxBOOSTAsyncForwardSocket;
}

void XBOOSTAsyncProxySocket::ResetForwardSocket()
{
	//m_pxBOOSTAsyncForwardSocket->Close();
	m_pxBOOSTAsyncForwardSocket.reset();
}

void XBOOSTAsyncProxySocket::SetServer()
{
	m_bServer = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

XBOOSTAsyncProxyAcceptor::XBOOSTAsyncProxyAcceptor()
{
}

XBOOSTAsyncProxyAcceptor::XBOOSTAsyncProxyAcceptor(int nReverseProxyPort, const std::wstring& szServerHost, int nServerPort)
	: m_pIOContext(new boost::asio::io_context()),
	m_pWork(new boost::asio::io_context::work(*m_pIOContext)),
	m_pThreadGroup(new boost::thread_group())
{
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor");
	std::cout << "XBOOSTAsyncProxyAcceptor" << std::endl;
	m_pEndpoint1 = GetEndpoint(nReverseProxyPort);
	m_pAcceptor = boost::make_shared<boost::asio::ip::tcp::acceptor>(*m_pIOContext, *m_pEndpoint1);
	//m_pAcceptor(new boost::asio::ip::tcp::acceptor(*m_pIOContext))

	m_szServerHost = szServerHost;
	m_nServerPort = nServerPort;
}

XBOOSTAsyncProxyAcceptor::~XBOOSTAsyncProxyAcceptor()
{
	OutputDebugStringA("~XBOOSTAsyncProxyAcceptor");
	std::cout << "~XBOOSTAsyncProxyAcceptor" << std::endl;
	AcceptorClose();
	Stop();
	//m_pIOContext->
}

void XBOOSTAsyncProxyAcceptor::Init(int nReverseProxyPort, const std::wstring& szServerHost,
	int nServerPort, const std::wstring& wszServiceName)
{
	//
	// Separate Init for Linux.
	//
	//XConfigServer& xConfigServer = XConfigServer::GetInstance();
	m_wszServiceName = wszServiceName;

	m_pIOContext = boost::make_shared<boost::asio::io_context>();
	m_pWork = boost::make_shared<boost::asio::io_context::work>(*m_pIOContext);
	m_pThreadGroup = boost::make_shared<boost::thread_group>();

	//m_nServerPort = nportNumber;
	//
	// The following 3 statement is not related Init of service mode
	// It is related thread pool and logs thread initialization
	// It should not be based on server mode init. thats why moved out.
	// and added more logs HCSRV-587
	InitThreadPool();
	//m_bServerRunning = true;
	//m_bServerUpdate = bCBAKeyUpdate;
	InitLogPurgerThread();

	m_pEndpoint1 = GetEndpoint(nReverseProxyPort);
	m_pAcceptor = boost::make_shared<boost::asio::ip::tcp::acceptor>(*m_pIOContext, *m_pEndpoint1);

	m_szServerHost = szServerHost;
	m_nServerPort = nServerPort;

	/*if (XServerWrapper::Init(xConfigServer, wszServiceName) == true) //HCSRV-481
	{
		XLog(xConfigServer).Print(log_sys, log_info, L"HALOCAD Service Successfully initialized.\n");
	}
	else
	{
		XStream<WCHAR> szMessage(wszServiceName.Str());
		szMessage << L" Initialization failed.\n";
		XLog(xConfigServer).Print(log_sys, log_info, szMessage.Str());
	}*/
}

void XBOOSTAsyncProxyAcceptor::InitThreadPool()
{
	//
	//Get the number of cores available in the current system.
	//
	unsigned int nNumberCores = boost::thread::hardware_concurrency();

	unsigned int nDefaultPoolMinSize = 10;
	unsigned int nDefaultPoolMaxSize = 50;

	const char* szNoThreads = "0";
	//const char* szNoThreads = getenv("HCS_BOOST_NO_THREAD");
	int nThreads = szNoThreads ? atoi(szNoThreads) : 0;

	//
	// We should have more than 1 thread to satisfy the incoming request.
	// If number of core is 1 it will be used only for LogPurge thread always.
	// So created 10 threads if the core is only one else number of threads 
	//  is equal to number of cores.
	//
	//UINT nThreadPoolSize = nNumberCores <=1 ? nDefaultPoolSize : nNumberCores;
	//UINT nThreadPoolSize = nDefaultPoolSize;

	// If the nNumberCores is 1 then thread pool size is 10, other wise thread pool size is 5 time number of cores.
	// If the thread pool size exceeds 50 then 50 will be the maximum thread pool size. This is under the condition
	// that environment variable for thread pool has not been set or it is 1. The environment variable for thread pool size is in the 
	// range 2 to 50.
	UINT nThreadPoolSize = 2;
	if (2 > nThreads)
		nThreadPoolSize = nNumberCores <= 1 ? nDefaultPoolMinSize : ((nNumberCores * 5) > nDefaultPoolMaxSize) ? nDefaultPoolMaxSize : (nNumberCores * 5);
	else if (nThreads > 1 && nThreads <= 50)
		nThreadPoolSize = nThreads;

	std::cout << "nThreadPoolSize:" << nThreadPoolSize << std::endl;

	for (UINT nThread = 0; nThread < nThreadPoolSize; ++nThread)
	{
		m_pThreadGroup->create_thread(boost::bind(&boost::asio::io_service::run, m_pIOContext));
	}
}

void XBOOSTAsyncProxyAcceptor::InitLogPurgerThread()
{
	// TODO: Need to call boost timer.
	m_pIOContext->post(boost::bind(&XBOOSTAsyncProxyAcceptor::LogPurger, this));
}

void XBOOSTAsyncProxyAcceptor::LogPurger()
{
	//XConfigServer& xConfigServer = XConfigServer::GetInstance();
	std::wstring szWInputMessage = L"Starting Log Purger Thread.";
	szWInputMessage += L"\n";
	//XLog(xConfigServer).Print(log_sys, log_info, szWInputMessage.c_str());
	try
	{
		//while (m_bServerRunning)
		{
			Sleep(5000);
			//XLog(xConfigServer).DeleteOldLogs();
		}
	}
	catch (...)
	{
		szWInputMessage = L"Exception Log Purger Thread.";
		szWInputMessage += L"\n";
		//XLog(xConfigServer).Print(log_sys, log_info, szWInputMessage.c_str());
	}

	szWInputMessage = L"Exiting Log Purger Thread.";
	szWInputMessage += L"\n";
	//XLog(xConfigServer).Print(log_sys, log_info, szWInputMessage.c_str());
}

void XBOOSTAsyncProxyAcceptor::Run()
{
	/*std::cout << "Run" << std::endl;
	boost::system::error_code ec;
	m_pIOContext->restart();
	try
	{
		m_pIOContext->run(ec);
	}
	catch (std::exception& ex)
	{

	}
	std::cout << "Run end" << std::endl;*/
	//XBOOSTAsyncProxyAcceptor xBOOSTAsyncAcceptor(5800, "127.0.0.1", 16365);
	Start();
	Accept();
}

void XBOOSTAsyncProxyAcceptor::Start()
{
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Start");
	//std::cout << "Start" << std::endl;
	m_pThreadGroup->create_thread(boost::bind(&XBOOSTAsyncProxyAcceptor::Run, this));
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Start end");
	//std::cout << "Start end" << std::endl;
}

void XBOOSTAsyncProxyAcceptor::Stop()
{
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Stop");
	//std::cout << "Stop" << std::endl;
	m_pIOContext->stop();
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Stop end");
	//std::cout << "Stop end" << std::endl;
}

void XBOOSTAsyncProxyAcceptor::AcceptorClose()
{
	boost::system::error_code ec;
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Close");
	//std::cout << "Close" << std::endl;
	m_pAcceptor->close(ec);
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Close end");
	//std::cout << "Close end" << std::endl;
}

boost::shared_ptr<boost::asio::ip::tcp::endpoint> XBOOSTAsyncProxyAcceptor::GetEndpoint(int nPort)
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

void XBOOSTAsyncProxyAcceptor::Accept()
{
	OutputDebugStringA("XBOOSTAsyncProxyAcceptor::Accept New");
	//std::cout << "Accept New" << std::endl;

	boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncPeerSocket =
		boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext, m_wszServiceName);
	m_pAcceptor->async_accept(pxBOOSTAsyncPeerSocket->GetSocket(),
		boost::bind(&XBOOSTAsyncProxyAcceptor::AcceptHandler, this, boost::asio::placeholders::error,
			pxBOOSTAsyncPeerSocket));

	//m_pAcceptor->async_accept(*(*pxBOOSTAsyncPeerSocket)(), *GetEndpoint(nPort),
	//	boost::bind(&XBOOSTAsyncServer::AcceptHandler, this, boost::asio::placeholders::error,
	//		pxBOOSTAsyncPeerSocket, nPort));

	OutputDbgStr("Peer", pxBOOSTAsyncPeerSocket.use_count());
	//std::cout << "Peer: " << pxBOOSTAsyncPeerSocket.use_count() << std::endl;
}

void XBOOSTAsyncProxyAcceptor::AcceptHandler(const boost::system::error_code& ec,
	boost::shared_ptr<XBOOSTAsyncProxySocket>& pXPeerSocket)
{
	OutputDebugStringA("AcceptHandler");
	//std::cout << "AcceptHandler" << std::endl;

	OutputDbgStr("Peer: ", pXPeerSocket.use_count());
	//std::cout << "Peer: " << pXPeerSocket.use_count() << std::endl;

	boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncNewPeerSocket =
		boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext, m_wszServiceName);
	m_pAcceptor->async_accept(pxBOOSTAsyncNewPeerSocket->GetSocket(),
		boost::bind(&XBOOSTAsyncProxyAcceptor::AcceptHandler, this, boost::asio::placeholders::error,
			pxBOOSTAsyncNewPeerSocket));

	if (ec == boost::system::errc::success)
	{
		pXPeerSocket->SetServer();

		// Create client
		OutputDebugStringA("Connect to PW server");
		//std::cout << "Connect to PW server" << std::endl;
		boost::shared_ptr<XBOOSTAsyncProxySocket> pxBOOSTAsyncNewClientSocket =
			boost::make_shared<XBOOSTAsyncProxySocket>(m_pIOContext, m_wszServiceName);
		OutputDbgStr("Client before connect : ", pxBOOSTAsyncNewClientSocket.use_count());
		//std::cout << "Client before connect: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;
		pxBOOSTAsyncNewClientSocket->Connect(m_szServerHost, m_nServerPort, pXPeerSocket);
		//Sleep(2000);
		OutputDbgStr("Client after connect : ", pxBOOSTAsyncNewClientSocket.use_count());
		//std::cout << "Client after connect: " << pxBOOSTAsyncNewClientSocket.use_count() << std::endl;

		OutputDebugStringA("AcceptHandler End");
		//std::cout << "AcceptHandler End" << std::endl;
	}
}

boost::shared_ptr<boost::asio::io_context>& XBOOSTAsyncProxyAcceptor::operator ()()
{
	return m_pIOContext;
}

void XBOOSTAsyncProxyAcceptor::JoinAll()
{
	m_pThreadGroup->join_all();
}

void XBOOSTAsyncProxyAcceptor::OutputDbgStr(const std::string& str)
{
	std::string wszStr;
	//wszStr = ((m_bServer) ? "Server " : "Client ");
	wszStr = str.c_str();
	OutputDebugStringA(wszStr.c_str());
}

void XBOOSTAsyncProxyAcceptor::OutputDbgStr(const long& lNum)
{
	std::string wszStr;
	//wszStr = ((m_bServer) ? "Server " : "Client ");
	wszStr = "";
	wszStr += lNum;
	OutputDebugStringA(wszStr.c_str());
}

void XBOOSTAsyncProxyAcceptor::OutputDbgStr(bool bServer, const long& lNum)
{
	std::string wszStr;
	wszStr = ((bServer) ? "Server " : "Client ");
	wszStr += lNum;
	OutputDebugStringA(wszStr.c_str());
}

void XBOOSTAsyncProxyAcceptor::OutputDbgStr(const std::string& str, const long& lNum)
{
	std::string wszStr;
	wszStr = str.c_str();
	wszStr += lNum;
	OutputDebugStringA(wszStr.c_str());
}

void _tmain()
{
	XBOOSTAsyncProxyAcceptor xBoostAsyncServer;
	xBoostAsyncServer.Init(5800, L"127.0.0.1", 20000, L"HCS");
	xBoostAsyncServer.Accept();

	std::cin.get();

	std::cout << "Stop" << std::endl;
	xBoostAsyncServer.Stop();

	std::cout << "JoinAll" << std::endl;
	xBoostAsyncServer.JoinAll();

	std::cout << "_tmain end" << std::endl;
}
