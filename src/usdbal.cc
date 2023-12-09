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

array<string,2> ignored_syms = { "DASH", "XMR" };
void load_config()
{
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
    
    money_t   usd;
    money_t   usd_goal;
    money_t   usd_del;
   
    pct_t     pct;
    pct_t     pct_goal;
    todo_t(const sym_t &sym=sym_t())
      :sym(sym)
    {
      xassert(!bal);
      xassert(!pct);
      xassert(!usd);
      xassert(!pct_goal);
      xassert(!usd_goal);
      xassert(!usd_del);
    };
    todo_t &operator+=(const todo_t &rhs)
    {
      if( rhs.sym == sym ) {
        bal+=rhs.bal;
      } else {
        bal=nan("mixed");
      };
      pct += rhs.pct;
      usd += rhs.usd;
      pct_goal += rhs.pct_goal;
      usd_goal += rhs.usd_goal;
      usd_del  += rhs.usd_del;
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
    res["usd"]=src.usd;
    res["pct"]=src.pct;
    res["pct_goal"]=src.pct_goal;
    res["usd_goal"]=src.usd_goal;
    res["usd_del"]=src.usd_del;
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
    if((lhs.usd_del) > (rhs.usd_del))
      return true;
    if((lhs.usd_del) < (rhs.usd_del))
      return false;
    return lhs.sym < rhs.sym;
  };
};
struct todo_less {
  bool operator()( const todo_t &lhs, const todo_t &rhs )
  {
    if((lhs.usd_del) < (rhs.usd_del))
      return true;
    if((lhs.usd_del) > (rhs.usd_del))
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
  auto monw=temp.usd.get_width();
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
    << "|" << setw( monw ) << "usd$"
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
  auto monw=rhs.usd.get_width();
  auto pctw=rhs.pct_goal.get_width();
  money_t spot_usd;
  if(rhs.sym=="Total") {
    spot_usd=spot_usd=nan("total");
  } else {
    spot_usd= market_t::conv(money_t(1), rhs.sym,"USD");
  };
  lhs
    << "|"
    << left
    << setw(symw) << rhs.sym << "|"
    << right
    << setw(monw) << rhs.bal << "|"
    << setw(monw) << spot_usd << "|"
    << setw(monw) << spot_usd << "|"
    << "|"
    << setw(pctw) << rhs.pct << "|"
    << setw(pctw) << rhs.pct_goal << "|"
    << setw(pctw) << (rhs.pct_goal-rhs.pct) << "|"
    << "|"
    << setw(monw) << rhs.usd*usd_spot() << "|"
    << setw(monw) << rhs.usd_goal*usd_spot() << "|"
    << setw(monw) << rhs.usd_del*usd_spot() << "|"
    << setw(monw) << rhs.usd
    << "|"
    ;
  return lhs;
};
void show_todos(const todo_t &usd, todo_v &todos, const todo_t &tot_all, const string &fname=string())
{
  cout << " " << header_t(usd,true) << endl;
  cout << " " << header_t(usd,false) << endl;
  cout << " " << header_t(usd,true) << endl;
  if(usd.sym.size())
    cout << " " << usd << endl;
  for( auto &todo : todos ) {
    cout << " " << todo << endl;
  };
  cout << "%" << tot_all << endl;
  cout << " " << header_t(usd,true) << endl;
  if(fname.size())
  {
    json fmt=json::parse("[]");
    fmt=todos;
    bittrex::save_json(fname,fmt,false);
  };
};
todo_m todo_map;
todo_v mk_todos()
{
  todo_map.clear();
  if( !goals.size() )
    xthrowre("you have no goals!");
  for ( auto &g : goals )
  {
    if(find(ignored_syms.begin(),ignored_syms.end(),g.first)!=ignored_syms.end())
      continue;
    todo_map[ g.first ].sym = g.first;
    todo_map[ g.first ].pct_goal = g.second;
  };
  money_t tot_usd = 0.0;
  money_t btc_spot= market_t::conv(1,"BTC","USDT");
  money_t btc_min_size = usd_min_size()/btc_spot;
  for ( auto &b : balance_l::load_balances() )
  {
    if (
        ( goals.find( b.sym ) != goals.end() )
        ||
        ( b.btc > btc_min_size )
       )
    {
      auto &todo=todo_map[ b.sym ];
      bool skipped =  find(ignored_syms.begin(),ignored_syms.end(),todo.sym)!=ignored_syms.end();
      todo.sym = b.sym;
      todo.usd = b.btc/btc_spot;
      todo.bal = b.bal;
      if(!skipped)
        tot_usd += todo.usd;
    };
  }
  {
    todo_v todos;
    for ( auto &item : todo_map ) {
      auto &sym=item.first;
      bool skipped =  find(ignored_syms.begin(),ignored_syms.end(),item.first)!=ignored_syms.end();
      auto &todo=item.second;

      if( skipped ) {
        todo.pct = 0;
        todo.usd_goal=todo.usd;
        todo.usd_del=0;
      } else {
        todo.pct = todo.usd/tot_usd;
        todo.usd_goal = tot_usd * todo.pct_goal.get();
        todo.usd_del = (todo.usd_goal - todo.usd);
      };
      if(todo.sym != "USDT")
        todos.push_back(todo);
    };
    {
      vector<sym_t> avoid;
      auto &usd=todo_map["USDT"];
      {
        if(usd.usd_del<0) {
          sort(todos.begin(),todos.end(),todo_more());
        } else {
          sort(todos.begin(),todos.end(),todo_less());
        }
        todo_t tot_all("Total");
        for( auto &todo : todos  )
          tot_all+=todo;
        tot_all+=usd;
        show_todos(usd, todos, tot_all,"etc/todos.json");
      };
      {
        todo_v willdo;
        todo_t tot_all("Total");
        for( auto &todo:todos ) {
          if( abs(todo.usd_del) >= usd_min_size() ) {
            tot_all+=todo;
            willdo.push_back(todo);
          };
        };
        if(willdo.size())
          show_todos(todo_t(),willdo,tot_all);
        return willdo;
      }
    };
  }
  return todo_v();
};
void adjust(const todo_t &todo)
{
};
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
  usd_spot(market_t::conv(1, "BTC","USDT"));
  auto todos=mk_todos();
  if(!todos.size() )
    return 0;

  sort(todos.begin(),todos.end(),todo_less());
  cout << " ----- " << endl;
  for(auto &todo:todos)
  {
    cout << todo.sym << " " << "BTC" << endl;
  };
  cout << " ----- " << endl;
  for(auto &todo:todos) {
    try {
    adjust(todo); 
    } catch ( const exception &ex ) {
      cout << ex << endl;
    };
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
