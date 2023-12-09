#include <web_api.hh>
#include <json.hh>
#include <fmt.hh>
#include <markets.hh>
#include <bittrex.hh>
#include <balance.hh>
#include <util.hh>
#include <order.hh>
#include <dbg.hh>
using namespace coin;
using namespace util;
using namespace std;
using namespace fmt;

const money_t min_txn_btc = 0.0006;
const money_t min_bal_btc = market_t::conv(money_t(5), "USDT", "BTC");

int xmain(int argc, char**argv)
{
  bittrex::dump_orders();
  return 0;
};
int main(int argc, char**argv) {
  try {
    int tty=xdup(1);
    xclose(0);
    xopen("/dev/null",O_RDONLY);
    xclose(1);
    mkdir("log",0755);
    int log=open_log("log/closeout.log",true);
    assert(log==1);
    xdup2(1,2);
    static fd_streambuf obuf(1,tty);
    static fd_streambuf ibuf(2,tty);
    cout.rdbuf(&obuf);
    cerr.rdbuf(&ibuf);
    cout << "log/closeout.log:1:started\n";
    vector<string> args(argv,argv+argc);
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

