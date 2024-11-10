// TestBoost.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
using namespace boost;
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
    string text = "token  test\tstring";
    
    std::cout << text << std::endl;

    char_separator<char> sep(" \t");
    tokenizer<char_separator<char> > tokens(text, sep);
    BOOST_FOREACH(string t, tokens)
    {
        cout << t << "." << endl;
    }
}

