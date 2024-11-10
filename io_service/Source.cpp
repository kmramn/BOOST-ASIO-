
#include <iostream>
#include <boost/asio.hpp>

//using namespace std;
//using namespace boost;

void main()
{
    boost::asio::io_service io_service;
    boost::asio::io_service::work work(io_service);

    for (int x = 0; x < 42; ++x)
    {
        io_service.poll();
        std::cout << "Counter: " << x << std::endl;
    }
}
