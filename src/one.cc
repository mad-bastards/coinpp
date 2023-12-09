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

void load_config()
{
  goals_t res;
  try {
    string text=util::read_file("etc/goals.json");
    json data=json::parse(text);
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
  } catch ( exception &e ) {
    cout << e << endl;
  };
  json ndata;
  ndata["goals"]=res;
  json ldata;
  ldata["usd_min_size"]=usd_min_size().get();
  ldata["usd_max_size"]=usd_max_size().get();
  ndata["limits"]=ldata;
  stringstream str;
  str << setw(4) << ndata;
  write_file("etc/goals.json.new",str.str());
};

using bittrex::simple_xact;

namespace coin {
  struct todo_t
  {
    sym_t     sym;
    money_t   bal;
    
    money_t   btc;
    money_t   btc_goal;
    money_t   btc_del;
   
    pct_t     pct;
    pct_t     pct_goal;
    todo_t(const sym_t &sym=sym_t())
      :sym(sym)
    {
      xassert(!bal);
      xassert(!pct);
      xassert(!btc);
      xassert(!pct_goal);
      xassert(!btc_goal);
      xassert(!btc_del);
    };
    todo_t &operator+=(const todo_t &rhs)
    {
      if( rhs.sym == sym ) {
        bal+=rhs.bal;
      } else {
        bal=nan("mixed");
      };
      pct += rhs.pct;
      btc += rhs.btc;
      pct_goal += rhs.pct_goal;
      btc_goal += rhs.btc_goal;
      btc_del  += rhs.btc_del;
      return *this;
    };
  };
  typedef std::vector<todo_t> todo_v;
  typedef std::map< sym_t, todo_t > todo_m;
  void to_json(json &dst, const todo_t &src);
};
void coin::to_json(json &dst, const todo_t &src)
{
  try {
    json res;
    res["sym"]=src.sym;
    res["bal"]=src.bal;
    res["btc"]=src.btc;
    res["pct"]=src.pct;
    res["pct_goal"]=src.pct_goal;
    res["btc_goal"]=src.btc_goal;
    res["btc_del"]=src.btc_del;
    dst=res;
  } catch ( const exception &ex ) {
    xtrace(ex.what());
    throw;
  };
};
using coin::todo_t;
using coin::todo_m;
using coin::todo_v;
struct todo_more {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) > (rhs.btc_del))
      return true;
    if((lhs.btc_del) < (rhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.btc_del) < (rhs.btc_del))
      return true;
    if((lhs.btc_del) > (rhs.btc_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
typedef vector<string> argv_t;
struct header_t {
  const todo_t &obj;
  bool dashes;
  header_t(const todo_t &obj, bool dashes)
    : obj(obj), dashes(dashes)
  {
  };
};

ostream &operator<<(ostream &lhs, const header_t &head)
{
  auto const& temp = head.obj;
  auto symw=temp.sym.get_width();
  auto monw=temp.btc.get_width();
  auto pctw=temp.pct_goal.get_width();
  ostringstream text;
  text
    << left
    << "|" << setw( symw ) << "SYM "
    << right
    << "|" << setw( monw ) << "rawBal"
    << "|" << setw( monw ) << "spot$ "
    << "|" << setw( monw ) << "spotB$ "
    << "|"
    << "|" << setw( pctw ) << "cur% "
    << "|" << setw( pctw ) << "goal% "
    << "|" << setw( pctw ) << "del%"
    << "|"
    << "|" << setw( monw ) << "cur$ "
    << "|" << setw( monw ) << "goal$ "
    << "|" << setw( monw ) << "del$"
    << "|" << setw( monw ) << "btc$"
    << "|";
  string str=text.str();
  if(head.dashes) {
    for(auto &ch : str)
    {
      if(ch=='|')
        ch='+';
      else
        ch='-';
    };
  };
  lhs << str;
  return lhs;
};
inline ostream &operator<<(ostream &lhs, todo_t rhs)
{
  auto symw=rhs.sym.get_width();
  auto monw=rhs.btc.get_width();
  auto pctw=rhs.pct_goal.get_width();
  money_t spot_btc;
  money_t spot_usd;
  assert(rhs.sym.size());
  if(rhs.sym=="Total") {
    spot_usd=spot_btc=nan("total");
  } else if ( rhs.sym == "" ) {
  } else {
    spot_btc= market_t::conv(money_t(1), rhs.sym,"BTC");
    spot_usd= market_t::conv(spot_btc,   "BTC",  "USD");
  };
  lhs
    << "|"
    << left
    << setw(symw) << rhs.sym << "|"
    << right
    << setw(monw) << rhs.bal << "|"
    << setw(monw) << spot_usd << "|"
    << setw(monw) << spot_btc << "|"
    << "|"
    << setw(pctw) << rhs.pct << "|"
    << setw(pctw) << rhs.pct_goal << "|"
    << setw(pctw) << (rhs.pct_goal-rhs.pct) << "|"
    << "|"
    << setw(monw) << rhs.btc*usd_spot() << "|"
    << setw(monw) << rhs.btc_goal*usd_spot() << "|"
    << setw(monw) << rhs.btc_del*usd_spot() << "|"
    << setw(monw) << rhs.btc
    << "|"
    ;
  return lhs;
};
void show_todos(const todo_t &btc, todo_v &todos, const todo_t &tot_all, const string &fname=string())
{
  cout << " " << header_t(btc,true) << endl;
  cout << " " << header_t(btc,false) << endl;
  cout << " " << header_t(btc,true) << endl;
  if(btc.sym.size())
    cout << " " << btc << endl;
  for( auto &todo : todos ) {
    cout << " " << todo << endl;
  };
  cout << " " << tot_all <<endl;
  cout << " " << header_t(btc,true) << endl;
  if(fname.size())
  {
    json fmt=json::parse("[]");
    fmt=todos;
    bittrex::save_json(fname,fmt,false);
  };
};
todo_m todo_map;
money_t usd( const money_t btc )
{
  return btc*usd_spot();
};
todo_t &btc() {
  return todo_map["BTC"];
};
todo_t mk_job()
{
  todo_map.clear();
  if( !goals.size() )
    xthrowre("you have no goals!");
  for ( auto &g : goals )
  {
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
  money_t tot_btc = 0.0;
  for ( auto &b : balance_l::load_balances() )
  {
    if (
        ( goals.find( b.sym ) != goals.end() )
        ||
        ( b.btc > btc_min_size() )
       )
    {
      auto &todo=todo_map[ b.sym ];
      todo.sym = b.sym;
      todo.btc = b.btc;
      todo.bal = b.bal;
      tot_btc += b.btc;
    };
  }
  todo_v todos;
  todo_t tot_all;
  todo_t &btc = todo_map["BTC"];
  for( auto &p : todo_map ) {
    auto &todo = p.second;
    todo.pct = todo.btc / tot_btc;
    todo.btc_goal = tot_btc * todo.pct_goal;
    todo.btc_del = todo.btc_goal - todo.btc;
    tot_all += todo;
    if(todo.sym != "BTC")
      todos.push_back(todo);
  };
  if(btc.btc_del < 0) {
    sort(todos.begin(),todos.end(),todo_more());
  } else {
    sort(todos.begin(),todos.end(),todo_less());
  };

  show_todos(btc, todos, tot_all,"etc/todos.json");
  todo_t chosen(todos.front());
  money_t tot_btc_del;
  if( btc.btc_del < 0 ) {
    tot_btc_del = chosen.btc_del-btc.btc_del;
  } else {
    tot_btc_del = -chosen.btc_del+btc.btc_del;
  };
  if( tot_btc_del < btc_min_size() ) {
    return todo_t();
  } else {
    return chosen;
  };
};

void adjust(todo_t &chosen)
{
  auto &btc=::btc();

  cout << btc << endl;
  cout << chosen << endl;
  money_t tot_btc_del;
  market_l mkts;
  market_t mkt;
  bool buy;
  money_t pri;
  if( btc.btc_del < 0 ) {
    tot_btc_del = chosen.btc_del-btc.btc_del;
    mkts = market_t::get(chosen.sym,btc.sym);
    mkt = mkts[0];
    buy = (mkt.sym()!=btc.sym);
  } else {
    tot_btc_del = -chosen.btc_del+btc.btc_del;
    mkts = market_t::get(btc.sym,chosen.sym);
    mkt = mkts[0];
    buy = (mkt.sym()==btc.sym);
  };
  pri = buy ? mkt.ask() : mkt.bid();
  cout << "market: " << mkt << endl;
  cout << "  " << btc.sym << " " << btc.btc_del << endl;
  cout << "  " << chosen.sym << " " << chosen.btc_del << endl;
  cout << "  " << "buy: " << boolalpha << buy << endl;
  simple_xact(mkt,buy,pri,tot_btc_del/2,true);
};
#define xverbose(x) xtrace(x)
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

  load_config();
  usd_spot(market_t::conv(1, "BTC","USD"));
  auto todo = mk_job();
  if( todo.sym.size() ) {
    adjust(todo);
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
