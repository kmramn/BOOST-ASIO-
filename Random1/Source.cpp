
#include <iostream>
#include <time.h>
#include <tchar.h>
#include <strsafe.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

void _tmain()
{
    boost::random::mt19937 gen(time(0));
    boost::random::uniform_real_distribution<> dist(0, 1);
    std::cout << dist(gen) << std::endl;
    std::cout << dist(gen) << std::endl;
    std::cout << dist(gen) << std::endl;
    std::cout << dist(gen) << std::endl;
    std::cout << dist(gen) << std::endl;
    std::cout << dist(gen) << std::endl;
}
