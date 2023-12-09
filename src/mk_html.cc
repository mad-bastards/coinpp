#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <balance.hh>
#include <stack>
#include <string_view.hh>

using namespace coin;
using namespace std;
using fmt::pct_t;
using fmt::nl;
using namespace std::literals::string_literals;

namespace myns {
int system(const char *cmd)
{
  return std::system(cmd);
};
int system(const string &cmd)
{
  return std::system(cmd.c_str());
};
};
//   struct html_stream
//   {
//     template<typename stream_t>
//     struct body_t
//       : public stream_t
//     {
//     };
//     body_t body;
//     html_stream( stream_t &&str )
//       : 
//   };

//   template<typename base_t, size_t reserve=sizeof(base_t)>
//   class wrapper_t
//   {
//     typedef array<char,reserve> buf_t;
//     buf_t buf;
//     void* raw()
//     {
//       return buf.data();
//     };
//     base_t *ptr()
//     {
//       return reinterpret_cast<base_t*>(raw());
//     };
//     base_t &ref()
//     {
//       return reinterpret_cast<base_t&>(*ptr());
//     };
//     template<typename con_t, typename... Args>
//       wrapper_t(const con_t &con, Args... args)
//       {
//         template<typename arg_t>
//   
//       };
//   };

struct tester_t
{
  constexpr static int num=42;
  constexpr static float pi=3.14159;
  constexpr static string_view str = { "test", 4 };
};
struct bracket_t
{
  ostream &stream;
  string obr, cbr;
  bracket_t(ostream &stream, const string &obr, const string &cbr)
    : stream(stream), obr(obr), cbr(cbr)
  {
    stream << obr;
  }
  ~bracket_t()
  {
    stream << cbr;
  };
};
ostream &operator <<(ostream &lhs, const tester_t &tester)
{
  lhs << typeid(tester);
  {
    bracket_t(lhs, "{", "}");
  };
  return lhs;
};
int xmain(int argc, char**argv)
{
  static vector<sym_t> ignore = { "USD", "XMR", "ETH", "XLM" };
  cerr << "Loading Balances" << endl;
  balance_l bals = balance_l::load_balances();
  {
    if( myns::system("rm -fr html.new") )
    {
      cerr << "failed to remove old temp dir" << endl;
      return 1;
    };
    mkdir("html.new",0755);
    if( myns::system("cp  css/index.css html.new/") )
    {
      cerr << "failed to copy index.css from css/" << endl;
      return 1;
    };
    ofstream html("html.new/index.html");
    html << ""
      "<html>\n"
      "  <head>\n"
      "    <link rel=\"stylesheet\" type=\"text/css\" href=\"index.css\">\n"
      "  </head>\n"
      "  <body>\n"
      ;
    for( auto const &bal : bals ) 
    {
      if( bal.addr == "" )
        continue;
      if( find(ignore.begin(),ignore.end(),bal.sym)!=ignore.end() )
        continue;
      html
        << "<div style=\"float: left\" >" << endl
        << "<h1>" << endl
        << "<center>" << endl
        << bal.sym << endl
        << "</center>" << endl
        << "</h1>" << endl
        << "<img src=\"" << bal.addr << ".png\">" << "<a>"
        << "</div>" << endl;
      string cmd="qrencode -ohtml.new/"+bal.addr+".png "+bal.addr;
      if( myns::system(cmd) != 0 ) {
        cerr << "image failed" << endl;
        return 1;
      };
    };
    html
      << "</body>" << endl
      << "</html>" << endl
      << endl;
  }
  if( myns::system("rm -fr html.old") ) {
    cerr << "failed to remove html.old" << endl;
    return 1;
  };
  if( myns::system("test -e html || exit 0; mv html html.old") ) {
    cerr << "failed to move html to html.old" << endl;
    return 1;
  };
  if( myns::system("mv html.new html") ) {
    cerr << "failed to move html to html.old" << endl;
    return 1;
  };
  if( myns::system("xdg-open html/index.html") != 0 ) {
    cerr << "failed to run xdg-open" << endl;
    return 1;
  };
  return 0;
};
int main(int argc, char**argv) {
  try {
    return xmain(argc,argv);
  } catch ( exception &e ) {
    cout << endl << endl << e.what() << endl << endl;
  } catch ( ... ) {
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "wtf?" << endl;
  };
  return -1;
};

