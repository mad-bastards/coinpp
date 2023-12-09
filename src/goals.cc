#include <bittrex_json.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>
#include <list>
#include <array>

using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;

typedef map<sym_t,double> goals_t;
goals_t goals;

static money_t _usd_min_size=10;
static money_t _usd_max_size=200;
static money_t _usd_spot;

const money_t & usd_spot()
{
  return _usd_spot;
};
const money_t & usd_spot(money_t new_spot)
{
  money_t res=_usd_spot;
  _usd_spot=new_spot;
  return _usd_spot;
};
const money_t & usd_min_size() {
  return _usd_min_size;
};
const money_t & usd_max_size() {
  return _usd_max_size;
};
money_t btc_max_size() {
  return usd_max_size()/usd_spot();
};
money_t btc_min_size() {
  return usd_min_size()/usd_spot();
};

set<sym_t> ignored_syms;

using bittrex::simple_xact;

typedef vector<string> argv_t;

int xmain( const argv_t &args )
{
  for( auto arg : args ) {
    if( arg == "-y" ) {
      bittrex::fake_buys=false;
      cout << "really gonna do it!" << endl;
    } else {
      cerr << "bad arg: " << arg << endl;
      exit(1);
    };
  };

  goals_t res;
  try {
    json data=json::parse(util::read_file("etc/goals.json"));
    json jgoals=data.at("goals");
    json jlims=data.at("limits");
    _usd_min_size=(double)jlims.at("usd_min_size");
    _usd_max_size=(double)jlims.at("usd_max_size");
    double sum=0;
    for( auto b(jgoals.begin()), e(jgoals.end()); b!=e; b++ )
    {
      double&val=res[sym_t(b.key())];
      val=(double)b.value();
      sum+=val;
    };
    cout << "sum of goals is " << sum << endl;
    for( auto &pair : res ) {
      pair.second/=sum;
    };
    goals=res;
    for( auto &goal : res ) {
      goal.second*=100000;
    };
    json ignore = data.at("ignored");
    for( auto b(ignore.begin()), e(ignore.end()); b!=e; b++ ) {
      sym_t sym = *b;
      ignored_syms.insert(sym);
    };
  } catch ( exception &e ) {
    cout << e << endl;
  };
  return 0;
};
// This is atest.
// This is only a test
int main( int argc, char** argv )
{
  try
  {
    mkdir("log",0700);
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
