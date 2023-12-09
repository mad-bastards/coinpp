#include <dbg.hh>
namespace dbg 
{
  using namespace std;
  ostream &show_dbg_msg(const char *file, unsigned line, const char *msg )
  {
    cerr << file << ":" << line << ":";
    if(msg)
      cerr << msg;
    return cerr;
  };
}
