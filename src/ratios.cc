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
struct date_t
{
  string text;
};
typedef tuple<sym_t,date_t,money_t,money_t,money_t,money_t,money_t> coin_data_t;

int xmain(int argc, char**argv)
{
  vector<string> ignore = { "USD" }; 
  cerr << "Loading Balances" << endl;
  balance_l bals = balance_l::load_balances();
  {
    mkdir("html",0755);
    ofstream html("html/page.html");
    cout
      << "<html>" << endl
      << "<body>" << endl;
    for( auto const &bal : bals ) 
    {
      if( bal.addr == "" )
        continue;
      if( find(ignore.begin(),ignore.end(),bal.sym)!=ignore.end() )
        continue;
      cout
        << "<div>" << endl
        << "<h3>" << endl
        << bal.sym << endl
        << "</h3>" << endl
        << "<a href=\"html/" << bal.addr << ".png\">" << endl
        << "</a>" << endl
        << "</div>" << endl;
      string cmd="qrencode -ohtml/"+bal.addr+".png "+bal.addr;
      if( myns::system(cmd) != 0 ) {
        cerr << "image failed" << endl;
        return 1;
      };

    };
    cout
      << "</body>" << endl
      << "</html>" << endl
      << endl;
  }
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

