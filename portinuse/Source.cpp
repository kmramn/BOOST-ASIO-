#include <stdio.h>
#include <iostream>
#include <boost\asio.hpp>
//#include <boost/thread.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/filesystem.hpp>
//#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/regex.hpp>
//#include <boost/asio/ip/tcp.hpp>

using namespace boost;
//using namespace boost::interprocess;
using boost::asio::ip::tcp;

bool accepting_connections(unsigned short port)
{
    using namespace boost::asio;
    using ip::tcp;
    using ec = boost::system::error_code;

    bool result = false;

    try
    {
        io_service svc;
        tcp::socket s(svc);
        deadline_timer tim(svc, boost::posix_time::seconds(1));

        tim.async_wait([&](ec) { s.cancel(); });
        s.async_connect({ {}, port }, [&](ec ec)
        {
            result = !ec;
        });

        svc.run();
    }
    catch (...) {}

    return result;
}

int main()
{
    //boost::regex e("^(\\w)+((\\w)*[-.]*(\\w)*)*(\\w)*@(\\w)+((\\w)*[-]*(\\w)*)*\\.(\\w)+");
    //regex_match("test@secude.halocore.com", e);
    //return 0;
    using namespace std;
    cout << tcp::endpoint(tcp::v4(), 30000).address().to_string() << endl;
    boost::asio::ip::address ip = boost::asio::ip::address::from_string("10.91.0.90");
    if (!ip.is_v4())
    {
        cout << "false" << endl;
        return false;
    }

#include <boost/asio.hpp>
    using boost::asio::ip::tcp;

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(boost::asio::ip::host_name(), "");
    tcp::resolver::iterator iter = resolver.resolve(query);
    tcp::resolver::iterator end; // End marker.
    while (iter != end)
    {
        tcp::endpoint ep = *iter++;
        std::cout << ep << std::endl;
    }
    return 0;
    //cout << tcp::v4() << endl;
    try
    {
        boost::shared_ptr< boost::asio::io_service > io_service(new boost::asio::io_service);
        tcp::acceptor acceptor(*io_service, tcp::endpoint(tcp::v4(), 49152));
        acceptor.close();
    }
    catch (...) {}

    return 0;
    if (accepting_connections(54435))
        cout << "Port 22 is accepting connections\n";
}