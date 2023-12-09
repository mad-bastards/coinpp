#include <bittrex_json.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>
#include <list>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;

typedef vector<string> argv_t;

struct screen_t {
  int col=0;
  int row=0;
  screen_t()
  {
    const char *tmp;
    tmp=getenv("LINES");
    if(tmp)
      row=lexical_cast<int>(tmp);
    tmp=getenv("COLUMNS");
    if(tmp)
      col=lexical_cast<int>(tmp);
    cout << "screen: " << row << " rows and " << col << " cols\n";
  };
} screen;

string ratio(const sym_t &lhs, const sym_t &rhs)
{
  ostringstream str;
  while(true) {
    if( lhs==rhs ) {
      str << "    x     ";
      break;
    };
    string pair=lhs+","+rhs;
    auto mkts=market_t::get(pair,pair,false);
    if(!mkts.size()){
      str << "          ";
      break;
    };
    money_t val1=1;
    money_t val2=mkts[0].last();
    if(val2<1)
    {
      val1/=val2;
      val2/=val2;
    }
    if( rhs == mkts[0].sym() )
      swap(val1,val2);
    do {
      str.str("");
      str << "[" << int(val1.get()) << ":" << int(val2.get()) << "]";
      val1*=10;
      val2*=10;
    } while(str.str().length()<11);
    break;
  };
  return str.str();
};
int xmain( const argv_t &args )
{
  auto &mkts = market_t::get_markets();
  vector<sym_t> syms= {
    "USDT", "BSV", "BCH", "BTC", "LINK", "ETH", "TRX", "EXP", "ZRX",
  };
  cout << __FILE__ << ":" << __LINE__ << ": fuck!" << endl;
  sort(syms.begin(),syms.end());
  market_l todo;
  cout << "|";
  cout << "       |";
  for( auto sym2 : syms ) {
    cout << " " << setw(7) << "" << setw(6) << sym2 << " |";
  };
  for( auto sym1 : syms ) {
    cout << endl;
    cout << left;
    cout << "| " << setw(5) << sym1 << " |";
    for( auto sym2 : syms )
    {
      cout << right << setw(14) << ratio(sym1,sym2);
      cout << " |";
    };
  };
  cout << endl;
//     cout << setfill('_') << left;
//     for( int i=0;i<pairs.size();i+=syms.size() )
//     {
//       cout << "|";
//       for(int j=0;j<syms.size();j++)
//       {
//         cout << " " << setw(10) << pairs[i+j] << " |";
//       };
//       cout << endl;
//     };
  return 0;
};
int main( int argc, char** argv )
{
  try
  {
    split_stream("log/bal.log");
    argv_t args( argv+1, argv+argc );
    if ( xmain( args ) )
      return 1;
    return 0;
  }
  catch ( exception& e )
  {
    cout << e << nl;
  }
  catch ( ... )
  {
    cout << nl << "wtf?" << nl;
  }
  return 1;
};
