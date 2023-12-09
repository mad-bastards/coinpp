#include <bittrex_json.hh>
#include <fmt.hh>
#include <json.hh>
#include <typeinfo>
#include <util.hh>
#include <web_api.hh>
#include <fcntl.h>
#include <util.hh>
#include <list>
#include <time.h>

const static string defDateTimeFmt="%04Y-%02m-%02d/%H-%M-%S" ;
string tm2str(struct tm &tm, const string &fmt=defDateTimeFmt)
{
  const size_t bufsize=1024;
  char buf[bufsize];
  if(strftime(buf,bufsize,fmt.c_str(),&tm)==0){
    return "overflow";
  } else {
    return buf;
  };
};
string time2str(time_t utime=time(0), const string &fmt=defDateTimeFmt)
{
  struct tm tm;
  if(localtime_r(&utime,&tm)){
    return tm2str(tm,fmt);
  } else {
    return "localtime_r failed";
  }
};
using namespace util;
using namespace coin;
using namespace fmt;

using namespace std;

using std::ostream;
using std::exception;
using std::type_info;
typedef vector<string> argv_t;

class brief
{
  string text;
  public:
  brief(const market_t &dat)
  {
    ostringstream temp;
    money_t pos=dat.high()+dat.low();
    
    temp << setprecision(4)
      << left
      << setw(8) << dat.cur()
      << setw(8) << dat.sym()
      << right
      << setw(20) << dat.high()
      << setw(20) << dat.last()
      << setw(20) << dat.low();
    text=temp.str();
  };
  static string hdr() {
    market_t mkt("sym-cur",0.00000001,0.00000002);
    brief brief(mkt);
    return brief.text;
  };
  const string str() const {
    return text;
  };
};
ostream &operator << (ostream &lhs, const brief &rhs)
{
  return lhs << rhs.str();
};
int xmain( const argv_t &args )
{
  auto &bals = balance_t::load_balances();
  auto &mkts = market_t::load_markets();

  string timestr=time2str();
  string hdr = timestr+" "+brief::hdr();
  for(auto b(hdr.begin()), e(hdr.end()); b!=e; b++){
    if(isspace(*b))
      continue;
    *b='-';
  };
  cout << hdr << endl;
  int num=1;
  for( auto mkt : mkts ) {
    if(mkt.sym()=="DASH")
      continue;
    if(mkt.sym()=="XMR")
      continue;
    if(mkt.sym()=="CTXC")
      continue;
    if(mkt.sym()=="USD") {
      xexpose(mkt);
      mkt=mkt.reverse();
    } else if ( mkt.cur()!="USD" ) {
      continue;
    };
    cout << timestr << setw(5) << num++ << " " << brief(mkt) << endl;
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
    split_stream("log/mkt.log");
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
