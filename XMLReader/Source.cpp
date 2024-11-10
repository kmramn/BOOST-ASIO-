
#define ATTR_SET ".<xmlattr>"
//#define XML_PATH1 "./pets.xml"
#define XML_PATH1 "./Temp.xml"

#include <Windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <strsafe.h>
#include <string>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace boost;
using namespace boost::property_tree;

const ptree& empty_ptree(){
    static ptree t;
    return t;
}

#define Int32x32To64(a, b)  (((__int64)((long)(a))) * ((__int64)((long)(b))))

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
    // Note that LONGLONG is a 64-bit value
    LONGLONG ll;

    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = ll >> 32;
}

#define BOOST_UTF8_BEGIN_NAMESPACE \
namespace boost { namespace detail {
#define BOOST_UTF8_DECL
#define BOOST_UTF8_END_NAMESPACE }}
#include <boost/detail/utf8_codecvt_facet.ipp>
#undef BOOST_UTF8_END_NAMESPACE
#undef BOOST_UTF8_DECL
#undef BOOST_UTF8_BEGIN_NAMESPACE

//#include <boost/detail/sp_typeinfo.hpp>
//#include <boost/locale.hpp>
//#include <locale>
//namespace loc = boost::locale;
int main()
{
    //loc::generator gen;
    //std::locale utf32_locale = gen.generate("en_US.utf-32");

    //std::locale utf32_locale = boost::locale::generator().generate("en_US.UTF-32");

    //std::locale utf8_locale(std::locale(), new boost::detail::utf8_codecvt_facet);
    ptree tree;
    read_xml(XML_PATH1, tree);
    //read_xml(XML_PATH1, tree, 0, utf8_locale);
    //read_xml(XML_PATH1, tree, 0, std::locale(""));

    //wptree tree;
    //read_xml(XML_PATH1, tree);

    //std:string szXML;
    //szXML = "<UserPermissions><mode>EMAIL</mode><email>administrator@qc-jenkins.com</email><!--1 or more repetitions:--><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission></UserPermissions><UserPermissions><mode>EMAIL</mode><email>administrator@qc-jenkins.com</email><!--1 or more repetitions:--><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission><user_permission>VIEW</user_permission></UserPermissions>";
    //std::stringstream ss;
    //ss << szXML;
    //read_xml(ss, tree);

    //cout << tree.count("users_permissions") << endl;
    //int  t3 =  tree.get("users_permissions", 4);
    //ptree t1 = tree.get_child("xml");
    /*wptree t1 = tree.get_child(L"XrML");
    BOOST_FOREACH(const wptree::value_type & f1, t1)
    {
        wcout << f1.first << " " << f1.second.data() << endl;
        wptree t2 = t1.get_child(f1.first);
        //string at = f1.first + ".<xmlattr>";
        //ptree t2 = t1.get_child(at);
        BOOST_FOREACH(const wptree::value_type & f2, t2)
        {
            wcout << f2.first << " " << f2.second.data() << endl;
            //ptree t2 = t1.get_child(f.first);
        }
    }*/
    const ptree & formats = tree.get_child("Test", empty_ptree());
    BOOST_FOREACH(const ptree::value_type & f, formats){
        string at = f.first + ATTR_SET;
        const ptree & attributes = formats.get_child(at, empty_ptree());
        cout << "Extracting attributes from " << at << ":" << endl;
        BOOST_FOREACH(const ptree::value_type &v, attributes){
            cout << "First: " << v.first.data() << " Second: " << v.second.data() << endl;
        }
    }
}

/*
#include <boost/locale.hpp>
using namespace boost::locale;
int main()
{
    generator gen;
    // Create locale generator 
    std::locale::global(gen("")); 
    // "" - the system default locale, set
    // it globally
}
*/