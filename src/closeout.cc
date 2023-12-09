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

//   const money_t min_txn_btc = 0.00055;
//   const money_t min_bal_btc = 4*min_txn_btc;

money_t _usd_min_txn, _usd_max_txn, _usd_btc_spot;
const money_t usd_min_txn() {
  xassert(_usd_min_txn);
  return _usd_min_txn;
};
const money_t usd_max_txn() {
  xassert(_usd_max_txn);
  return _usd_max_txn;
};
const money_t usd_min_bal() {
  return 2*usd_min_txn();
};
const money_t usd_btc_spot() {
  return _usd_btc_spot;
};
const money_t btc_usd_spot() {
  return 1/_usd_btc_spot;
};
const money_t btc_max_txn() {
  return usd_max_txn()/usd_btc_spot();
};
const money_t btc_min_bal() {
  return usd_min_bal()/usd_btc_spot();
};
const money_t btc_min_txn() {
  return usd_min_txn()/usd_btc_spot();
};

typedef map<sym_t,pct_t> goals_t;
goals_t goals;
void load_config() {
    string text=util::read_file("etc/goals.json");
    json data=json::parse(text);
    json jgoals=data.at("goals");
    json jlims=data.at("limits");
    _usd_min_txn=(double)jlims.at("usd_min_size");
    _usd_max_txn=(double)jlims.at("usd_max_size");

    pct_t tot; 
    for( auto b(jgoals.begin()), e(jgoals.end()); b!=e; b++ )
    {
      pct_t pct =(double)b.value();
      goals[sym_t(b.key())]=pct;
      tot+=pct;
    };
    for( auto &goal : goals ) {
      goal.second = goal.second / tot;
      cout << goal.first << " " << goal.second << endl;
    };
};
//  string simple_xact ( const market_t &market, bool buy, money_t qty, money_t rate, bool ioc);
//#define xcheckin(x)
// using bittrex::simple_xact;
//   string bittrex::simple_xact (
//    const market_t &market,
//    bool buy,    // buy or sell
//    money_t qty, // in the symbol to buy or sell
//    money_t rate, // in currency per unit
//    bool ioc
//    )
bool adjust(const balance_t &bal)
{
  xtrace("adjust: " << bal);

  auto mkt = market_t::get(bal.sym,"BTC")[0];
  if(bal.btc < btc_min_txn()) {
    cout << "Buy, then sell" << endl;
    xexpose(mkt.bid());
    xexpose(mkt.ask());
  } else if ( bal.btc < btc_min_bal() ) {
    cout << "sell" << endl;
    xexpose(mkt.bid());
    xexpose(mkt.ask());
  } else {
    xthrowre("And you may ask yourself \"How did I get here?\"");
  };

//     string res;
//     if( bal.btc < min_txn_btc ) {
//       xtrace("Buy Then Sell");
//       if( mkt.sym() == bal.sym ) {
//         money_t rate = mkt.ask();
//         money_t qty = min_txn_btc/rate;
//         xexpose(rate);
//         xexpose(qty);
//      string res=bittrex::simple_xact( mkt, true, qty, rate, false ); 
//      cout << res << endl;
//         return true;
//       } else {
//         money_t rate = mkt.bid();
//         money_t qty = min_txn_btc*rate;
//         xexpose(rate);
//         xexpose(qty);
//      string res=bittrex::simple_xact( mkt, false, qty, rate, false ); 
//      cout << res << endl;
//         return true;
//       };
//     } else if ( bal.btc < min_bal_btc ) {
//       xtrace("we have enough " << bal.sym << " to sell, but not enough to keep");
//       if( mkt.sym() == bal.sym ) {
//         cout << "yes" << endl;
//       } else {
//         money_t rate=mkt.ask();
//         money_t q = bal.bal/rate;
//         money_t qty;
//         ((qty=q));
//         ((qty=q/1.0015));
//   //     res=bittrex::simple_xact( mkt, true, qty, rate, false ); 
//   //      cout << res << endl;
//         return true;
//       };
//     } else {
//       xtrace("And you may ask yourself ... how did I get here?");
//     };
  return false;
};
string prog_name;
int xmain(int argc, char**argv)
{
  prog_name=argv[0];
  for( int i=1;i<argc;i++){
    if(argv[i]==string("-y"))
      bittrex::fake_buys=false;
    else
      xthrowre("no idea what '"<<argv[i]<<"' means");
  };
  load_config();

  _usd_btc_spot=market_t::get("BTC","USD")[0].last();
  xexpose(usd_btc_spot());
  xexpose(btc_usd_spot());
  auto bals=balance_t::get_balances();
  sort(bals.begin(),bals.end(),std::greater<balance_t>());
  bool min_txn_shown=false;
  bool min_bal_shown=false;
  for( auto &bal : bals )
  {
    if(!bal.bal) {
      //cout << bal.sym << ": 0 balance" << endl;
      continue;
    };
    if(bal.pend) {
      //cout << bal.sym << ": transaction pending" << endl;
      continue;
    };
    if(bal.btc > btc_max_txn()) {
      continue;
    };
    if(goals[bal.sym]) {
      continue;
    };
    if(bal.btc >= btc_min_txn()) {
      if(!min_txn_shown)
        xexpose(btc_min_txn());
      min_txn_shown=true;
    };
    if(bal.btc >= btc_min_bal()) {
      if(!min_bal_shown)
        xexpose(btc_min_bal());
      min_bal_shown=true;
    };
    cout << bal << endl;
    adjust(bal);
    break;
  };
  if(!min_txn_shown)
    xexpose(btc_min_txn());
  if(!min_bal_shown)
    xexpose(btc_min_bal());
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

