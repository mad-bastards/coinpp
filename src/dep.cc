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

int xmain( const argv_t &args )
{
  bittrex::show_deposits();
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
