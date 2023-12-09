#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>
#include <list>
#include <array>
#include <symbol.hh>
#include <money.hh>

using namespace util;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;
using namespace coin;

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
void load_config()
{
  const string text;
  try {
    json data=json::parse(util::read_file("etc/phrase.json"));
    cout << setw(4) << data << endl;
  } catch ( exception &e ) {
    cerr << e << endl;
    ostringstream text;
    const string abandon = util::quote("abandon");
    const string about = util::quote("about");
    
    for(int i=0;i<11;i++){
      text << abandon << ", ";
    }
    text << about;

    util::write_file("etc.phrase.json",text.str());
  };
};

typedef vector<string> argv_t;

template<typename val_t>
int sign(const val_t &val)
{
  if(!val)
    return 0;
  if(val==abs(val))
    return 1;
  else
    return -1;
};

#define xverbose(x) xtrace(x)
int xmain( const argv_t &args )
{
  bool first=true;
  for( auto arg : args ) {
    if(!first)
      cout << "," << endl;
    cout << "  " << arg;
  };
  cout << endl;
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
