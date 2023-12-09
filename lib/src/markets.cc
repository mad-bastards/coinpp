#include <markets.hh>
#include <web_api.hh>
#include <util.hh>
#include <bittrex.hh>
#include <dbg.hh>

using namespace util;
using namespace coin;
using namespace fmt;
market_l coin::market_t::markets;
coin::market_t::market_t()
{
};
coin::market_t::market_t(const market_t &rhs)
  : data(rhs.data)
{
};
coin::market_t::market_t(const data_t &data)
  : data(data)
{
  if(cur()+"-"+sym() != name())
    xthrow(
        runtime_error,
        "cur(" << cur() << ")+'-'+sym(" << sym() << ")!=name" << name() << ")"
        );
  if(bid()>=ask())
    xthrow(runtime_error,"bid(" << bid() << ")>=ask(" << ask() << ")");
};
coin::market_t::market_t(const string &name, money_t bid, money_t ask)
{
  try {
    data_t tmp;
    tmp.name=name;
    split_name(tmp.name,tmp.cur,tmp.sym);
    tmp.bid=bid;
    tmp.ask=ask;
    xassert(bid>0 && bid<ask);
    data=tmp;
  } catch ( const exception &ex ) {
    xthrowre
      (
       "while processing market '" << name << "' (bid=" << bid << ", ask=" << ask << "\n" << 
       ex.what() << endl
      );
  };
};
market_t market_t::reverse() const {
  data_t tmp;
  tmp.cur=data.sym;
  tmp.sym=data.cur;
  tmp.name=tmp.cur+"-"+tmp.sym;
  tmp.ask=1/data.bid;
  tmp.bid=1/data.ask;
  tmp.low=1/data.high;
  tmp.high=1/data.low;
  tmp.last=1/data.last;
  tmp.avg=1/data.avg;
  tmp.vol=data.vol;

  xassert(tmp.bid>0 && tmp.bid<tmp.ask);
  market_t mkt(tmp);
  cout << mkt << endl;
  return mkt;
};
const market_l& market_t::get_markets()
{
  if(!markets.size())
    load_markets();
  if(!markets.size())
    xthrow(runtime_error,"failed to load markets!");
  return markets;
};
const market_l &market_t::load_markets() {
  markets=bittrex::load_markets();
  if(!markets.size())
    xthrow(runtime_error,"failed to load markets!");
  return markets;
};
money_t market_t::conv(money_t amount, const sym_t &from, const sym_t &to) {
  if(from==to)
    return amount;
  market_l list=market_t::get(from,to);
  assert(list.size());
  auto &mkt=list.back();
  if(false)
    xexpose(mkt);
  return mkt.yield(amount,from,to);
};
money_t market_t::conv2(money_t amount, const sym_t &from, const sym_t &to) {
  if(from==to)
    return amount;
  market_l list=market_t::get(from,to,false);
  if(list.size())
    return conv(amount, from, to);
  amount=conv(amount,from,"BTC");
  amount=conv(amount,"BTC",to);
  return amount;
};
market_l market_t::get(const string &f, const string &t, bool except)
{
  market_l res;
  bool verbose=false;
  if(f == "Total" || t == "Total")
    xthrowre("WTF?");
  set<sym_t> syms;
  for(auto sym :  split(',',f+","+t))
  {
    syms.insert(sym_t(sym));
  };
  if(verbose) {
    string space="";
    for( auto sym :syms )
    {
      cout << space << sym;
      space="  ";
    };
    cout << endl;
  };
  for( auto const &m : get_markets() )
  {
    if(verbose) {
      xexpose(m.name());
    };
    if(syms.count(m.sym()) and syms.count(m.cur()))
      res.push_back(m);
  };
  if( except && ( res.size() ==0  ) )
    xthrow(runtime_error,"cannot find market for "<<f<<" and "<<t);
  return res;
};
static const sym_t no_sym;
ostream &coin::market_l::stream( ostream &res, int ind ) const
{
  res << setw(ind) << "{" << endl;
  for( auto &mkt : *this ) {
    mkt.stream(res,ind+2);
  };
  res << setw(ind) << "}" << endl;
  return res;
};
ostream &coin::market_t::stream( ostream &res, int ind ) const
{
  res << setw(ind+2) << "" <<"name: " << data.name << endl;
  money_t unit(1);
  res << setw(ind+2) << "" <<"bid:  " << data.bid << " + " << (data.bid*0.002) << endl;
  res << setw(ind+2) << "" <<"ask:  " << data.ask << " - " << (data.ask*0.002) << endl;
  money_t val=(data.bid+data.ask)/2;
  res << setw(ind+2) << "" <<"val:  " << val << endl;
  pct_t spread=(data.ask-data.bid)/val;
  res << setw(ind+2) << "" <<"spr:  " << spread << endl;
  res << setw(ind+2) << "" <<"day:  " << data.prev << endl;
  {
    money_t rate=(data.ask*1.0020);
    if( rate < 1 ) {
      rate=1/rate;
      res<< setw(ind+2)<<""<<" BUY  " << rate << data.sym <<" for " << unit << data.cur;
    } else {
      res<< setw(ind+2)<<""<<" buy  " << unit << data.sym <<" for " << rate << data.cur;
    }
  };
  res << endl;
  {
    money_t rate(data.bid/1.0020);
    if( rate < 1 ) {
      rate=1/rate;
      res<< setw(ind+2)<<""<<" SELL " << rate <<data.sym<<" for " << unit << data.cur;
    } else {
      res<< setw(ind+2)<<""<<" sell " << unit <<data.sym<<" for " << rate << data.cur;
    }
  };
  res << endl;
  return res;
};
money_t market_t::yield(money_t qty, sym_t f, sym_t t, bool neutral)
{
  money_t rate;
  if( neutral )
    rate=(bid()+ask())/2;
  if( f == data.cur && t == data.sym ) {
    // This is a buy
    if( !neutral )
      rate=bid();
    return qty/rate;
  } else if ( f == data.sym && t == data.cur ) {
    // This is a sell
    if( !neutral )
      rate=ask();
    return qty*rate;
  } else {
    xthrow(runtime_error,
        "market " << name() << " cannot convert from " << f << " to " << t);
  };
};
bool market_t::split_name(const string &name, string &cur, string &sym)
{
  do {
    const auto b(name.begin()), e(name.end());
    auto s(find(b,e,'-'));
    cur.assign(b,s);
    sym.assign(++s,e);
    assert(cur.length()>1);
    assert(sym.length()>1);
    return true;
  } while(false);
  cur.clear();
  sym.clear();
  return false;
};
bool market_t::operator<(const market_t &rhs) const
{
  if(data.cur<rhs.data.cur)
    return true;
  else if (data.cur>rhs.data.cur)
    return false;
  else if(data.sym<rhs.data.sym)
    return true;
  else if(data.sym>rhs.data.sym)
    return false;
  return data.last < rhs.data.last;
}

#define empty_des(x) x::~ x(){}
//   empty_des(can_str);
//   empty_des(market_path);
//   empty_des(market_path_l);

