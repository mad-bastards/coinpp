#include <bittrex_json.hh>
#include <util.hh>
#include <json.hh>
#include <web_api.hh>
#include <cmath>
#include <dbg.hh>
#include <sstream>

using namespace coin;
using namespace util;
using namespace fmt;
using namespace web;
using std::ostringstream;
using nlohmann::detail::value_t;

#if 1
#define trace_from_json(x)
#else
#define trace_from_json(x) xtrace(x)
#endif
#if 1
#define trace_to_json(x)
#else
#define trace_to_json(x) xtrace(x)
#endif
namespace bittrex {
  extern bool fake_loads;
};
bool bittrex::fake_loads=false;
bool bittrex::fake_buys=true;
bool bittrex::show_urls=true;
const static string api_url = "https://bittrex.com/api/v1.1/";
using web::load_page;
void bittrex::save_json(const string &fname, const json &json, bool backup)
{
  trace_from_json(__PRETTY_FUNCTION__ << ": fname=" << fname);
  assert(fname.length());
  ofstream ofile;
  {
    int fd=-1;
    if(  backup ) {
      fd=util::open_log(fname);
    } else {
      unlink(fname.c_str());
      fd=util::xopen(fname.c_str(),O_WRONLY|O_CREAT);
    };
    ofile.open(fname,ios::app);
    xclose(fd);
  };
  if(!ofile)
    xthrowre("open:"+fname+strerror(errno));
  ofile<<setw(4)<<json<<endl<<endl;
  if(!ofile)
    xthrowre("error writing "+fname);
};
const json bittrex::load_json(const string &url, const string &save_to, bool backup)
{
  trace_from_json(__PRETTY_FUNCTION__ << ": url=" << url << " save_to+" << save_to);
  try {
    string page;
    if( bittrex::fake_loads ) {
      xtrace("using cached page at: " << save_to);
      ifstream str(save_to);
      if(!str)
        xthrowre("open:"+save_to+":"+strerror(errno));
      string line;
      while(getline(str,line))
        page+=line;
    } else {
      page = web::load_sec_page(url,"");
      xassert(page.length());
      const char *text=page.c_str();
      cout << "page:" << endl;
      cout << text << endl;
      cout << "page:" << endl;
    };
    json jpage=json::parse(page);
    save_json(save_to,jpage,backup);
    if(!jpage.at("success")) {
      throw runtime_error(
          "no success in result\n\n"+page
          );
    };
    jpage=*jpage.find("result");
    return jpage;
  } catch ( exception &ex ) {
    xtrace(ex.what());
    xexpose(url);
    xexpose(save_to);
    throw;
  } catch ( ... ) {
    throw;
  };
};
string bittrex::json_str(order_t const &ord)
{
  trace_to_json(__PRETTY_FUNCTION__);
  json res;
  coin::to_json(res,ord);
  string str=res.dump(4);
  return str;
};
#define list( item )                                  \
  item(  string, uuid,                 "OrderUuid"          ) \
  item(  string, exchange,             "Exchange"           ) \
  item(  string, type,                 "Type"               ) \
  item(  string, type,                 "OrderType"          ) \
  item(  string, opened,               "Opened"             ) \
  item(  string, closed,               "Closed"             ) \
  item(  bool,   is_open,              "IsOpen"             ) \
  \
  item(  money_t, limit,                "Limit"              ) \
  item(  money_t, quantity,             "Quantity"           ) \
  item(  money_t, price,                "Price"              ) \
  item(  money_t, price_per_unit,       "PricePerUnit"       ) \
  item(  money_t, quantity_remaining,   "QuantityRemaining"  ) \
  \
  item(  bool,    cancel_initiated,     "CancelInitiated"    ) \
  \
  item(  money_t, commission_paid,      "CommissionPaid"     ) \
  item(  money_t, commission_remain,    "CommissionReserveRemaining" ) \
  item(  money_t, commission_reserved,  "CommissionReserved" ) \
  \
  item(  bool,    is_conditional,       "IsConditional"      ) \
  item(  string,  condition,            "Condition"          ) \
  item(  string,  condition_target,     "ConditionTarget"    ) \
  \
  item(  bool,    immediate_or_cancel,  "ImmediateOrCancel"  ) \
  item(  string,  reserved,             "QuantityRemaining"  ) \
  item(  string,  sentinel,             "Sentinel"           ) \
  nop()

void coin::from_json(const json &j, order_t& o )
{
  order_t::data_t tmp;
#define extract( t, x, y )  if(j.find(y)!=j.end()) { coin::from_json(j.at(y),tmp.x); }
  list( extract );
#undef extract
  o=order_t(tmp);
};
void coin::to_json  (      json &j, const order_t &o )
{
  trace_from_json(__PRETTY_FUNCTION__);
  json res;
#define export( t, x, y )  res[y]=o.get_data().x;
list(export);
#undef export
  j=res;
};
void coin::to_json  (      json &j, const balance_t &val)
{
  json temp;
  temp["Currency"]=val.sym;
  temp["Balance"]=val.bal;
  temp["Available"]=val.ava;
  temp["Pending"]=val.pend;
  j=temp;
};
void fmt::to_json  (      json &j, const pct_t &val)
{
  trace_from_json(__PRETTY_FUNCTION__<<":"<<val);
  ostringstream str;
  str << setprecision(8) << fixed << val.get();
  j=lexical_cast<double>(str.str());
};
void coin::to_json  (      json &j, const money_t &val)
{
  trace_from_json(__PRETTY_FUNCTION__<<":"<<val);
  ostringstream str;
  str << setprecision(8) << fixed << val.get();
  j=lexical_cast<double>(str.str());
};
//   void coin::to_json  (      json &j, const market_t &ml);
void coin::to_json  (      json &j, const order_l& orders )
{
  trace_from_json(__PRETTY_FUNCTION__);
  json temp;
  for( auto &order : orders )
  {
    json jorder;
    coin::to_json(jorder,order);
    temp.push_back(jorder);
  };
};
void coin::to_json  (      json &j, const string &val)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << val);
  j=val;
};
void coin::to_json  (      json &j, const bool &val)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << val);
  j=val;
};
void coin::from_json(const json &json, order_l& o )
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << json);
  order_l tmps;
  for( auto const &j : json ) {
    order_t tmp;
    coin::from_json(j,tmp);
    tmps.push_back(tmp);
  };
  swap(tmps,o);
};
void coin::from_json(const json &j, bool &val)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if(j.is_null())
    val=false;
  else
    j.get_to(val);
};
void coin::from_json(const json &j, string &val)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if(j.is_null())
    val=string();
  else if(j.is_number())
    val=lexical_cast<string>((double)j);
  else
    j.get_to(val);
};


//   #define display( x, y )                                                   \
//     cout << left << setw( 20 ) << y << right << setw( 20 ) << "*" << o.x    \
//          << endl;
//       list( display );
//     ;
void coin::from_json(const json &j, market_l &ml)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  for( auto it = j.begin(); it != j.end();it++ ) 
  {
//       try {
      market_t tmp;
      coin::from_json(*it,tmp);
      ml.push_back(tmp);
//       } catch ( exception &ex ) {
//         xcarp(ex.what());
//         xcomment("in json: " << setw(4) << *it);
//       };
  };
};
void coin::from_json(const json &j, money_t &val) {
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if( j.is_null() )
    val=0;
  else 
    val=(double)j;
};
void coin::from_json(const json &j, balance_t &bal) {
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  xassert(!j.is_null());
  try {
    balance_t res;
    from_json(j.at("Currency"),res.sym);
    from_json(j.at("Balance"),res.bal);
    from_json(j.at("Available"),res.ava);
    from_json(j.at("Pending"),res.pend);
    from_json(j.at("Pending"),res.addr);
    if(j.at("CryptoAddress").is_null()) {
      res.addr="";
    } else {
      from_json(j.at("CryptoAddress"),res.addr);
    };
    try {
      if(res.bal)
        res.btc=market_t::conv(res.bal,res.sym,"BTC");
    } catch ( exception &ex ) {
      xtrace(ex.what());
      throw;
    } catch ( ... ) {
      throw;
    };
    bal=res;
  } catch(exception &ex) {
    cerr << "while reading json: " << setw(4) << j << endl;
    xexpose(ex.what());
    throw;
  };
};
void coin::to_json  (      json &j, const market_t &m)
{
  try {
    json tmp;
    tmp["MarketName"]=m.name();
    tmp["Bid"]=m.bid();
    tmp["Ask"]=m.ask();
    tmp["Last"]=m.data.last;
    j=tmp;
    return;
  } catch ( const exception &ex ) {
    xcomment(ex.what());
    throw;
  }

}
std::vector<string> skips;
void coin::from_json(const json &j, market_t &m)
{
  trace_from_json(__PRETTY_FUNCTION__ << ":" << setw(4) << j);
  if( skips.empty() ) {
    string text = read_file("etc/badstuff.txt");
    vector<string> toks = util::ws_split(text);
    skips.push_back("BTC-BTC");
    for( auto &tok : toks ) {
      skips.push_back(tok);
    };
    if(skips.size()) {
      cout << "skips:";
      auto b(skips.begin()), e(skips.end());
      while(b!=e)
        cout << " " << *b++;
      cout << endl;
    };
  };
  try {
    xassert(!j.is_null());
    string name=j.at("MarketName");
    if(j.at("Bid").is_null() || j.at("Ask").is_null()) {
      if(find(skips.begin(),skips.end(),name)==skips.end()){
        skips.push_back(name);
      };
      return;
    };
    money_t bid=j.at("Bid");
    money_t ask=j.at("Ask");
    if(bid<=0 || bid>=ask ) {
      if(find(skips.begin(),skips.end(),name)==skips.end())
      {
        xcomment("bad bid/ask for " << name << " skipping");
        xcomment("     bid: " << bid << " ask: " << ask);
      };
      return;
    };
    market_t tmp(name,bid,ask);
    coin::from_json(j.at("Last"),tmp.data.last);
    coin::from_json(j.at("High"),tmp.data.high);
    coin::from_json(j.at("Low"),tmp.data.low);
    coin::from_json(j.at("Volume"),tmp.data.vol);
    double prev=0;
    j.at("PrevDay").get_to(prev);
    tmp.data.prev=prev;
    m=tmp;
    return;
  } catch ( const exception &ex ) {
    xcomment(ex.what());
    return;
  }
};

string bittrex::simple_xact (
 const market_t &market,
 bool buy,    // buy or sell
 money_t qty, // in the symbol to buy
 money_t rate, // in currency per unit
 bool ioc
 )
{
  string act=(buy?"buy":"sell");
  xassert(qty>=0);
  money_t total=qty*rate;
  cout 
    << setw(5) << act
    << "  " << qty 
    << "  " << market.sym()
    << " at " << rate << market.cur() << "/unit"
    << " on " << market.data.name
    << " for " << total
    << " ioc: " << ioc
    << endl
    ;
  const static string sl_url=
    "https://bittrex.com/api/v3/market/";
  string url=sl_url+act+"limit?";
  url+="market="+market.data.name;
  url+="&quantity="+strip(lexical_cast<string>(qty));
  url+="&rate="+strip(lexical_cast<string>(rate));
  if(ioc)
    url+="&timeInForce=IOC";
  url+="&";
  if(show_urls)
    cout << "url: " << url << endl;
  if(fake_buys || fake_loads)
    return "faked";

  try {
    string page = web::load_sec_page(url,"");
    auto jpage=json::parse(page);
    cout << setw(4) << jpage << endl;
    if(!jpage.at("success")) {
      throw runtime_error( "no success in buylimit result\n\n"+page);
    };
    jpage=jpage.at("result");
    cout << setw(4) << jpage << endl;
    jpage=jpage.at("uuid");
    cout << setw(4) << jpage << endl;
    return jpage;
  } catch ( const exception &ex ) {
    ostringstream msg;
    msg << "got exception: " << ex << endl;
    msg << "           in: " << __PRETTY_FUNCTION__ << endl;
    msg << "          url: " << url << endl;
    throw runtime_error(msg.str());
  };
};
void bittrex::show_withdrawals() {
  cout << "with" << endl;
  const static string gw_url=
    "https://bittrex.com/api/v3/account/getwithdrawalhistory?";
  string page = web::load_sec_page(gw_url,"");
  //	cout << string('-',20) << endl;
  cout << "--------------" << endl << page << endl << endl;
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getwithdrawalhistory result\n\n"+page
        );
  };
  cout << endl << endl;
  cout << setw(4) << jpage;
  cout << endl << endl;
};
void bittrex::show_deposits() {
  cout << "dep" << endl;
  const static string gd_url=
    "https://bittrex.com/api/v3/account/getdeposithistory?";
  string page = web::load_sec_page(gd_url,"");
  //	cout << string('*',20) << endl;
  json jpage=json::parse(page);
  if(!jpage.at("success")) {
    throw runtime_error(
        "no success in getdeposithistory result\n\n"+page
        );
  };
  cout << endl << endl;
  cout << setw(4) << jpage;
  cout << endl << endl;
};
const coin::balance_l bittrex::load_balances()
{
  const static string gb_url=
    "https://bittrex.com/v3/balances";

  json jpage = load_json(gb_url,"logs/balances.json");
  balance_l temp;
  for( json bal : jpage ) {
    cout << bal["Currency"] << endl;
    if(bal.at("Currency")=="BTXCRD")
      continue;
    balance_t obj(bal);
    temp.push_back(obj);
  };
  sort(temp.begin(),temp.end());
  return balance_l(temp.rbegin(),temp.rend());
};
const market_l bittrex::load_markets()
{
  const static string gms_url=
    "https://bittrex.com/api/v1.1/public/getmarketsummaries?";
  json jpage = load_json(gms_url,"log/markets.json");
  market_l markets;
  coin::from_json(jpage,markets);
  cout << markets.size() << " markets loaded" << endl;
  market_l selected;
  for( auto &market : markets )
  {
//    if( market.data.cur == "BTC" || market.data.sym=="BTC" )
      selected.push_back(market);
  };
  cout << selected.size() << " markets selected" << endl;
  return selected;
};
void bittrex::cancel_order(const string &id)
{
  const static string b_url("https://bittrex.com/api/v1.1/market/cancel?");
  string url = b_url+"uuid="+id+"&";
  json page = load_json(url,"log/cancel.json");
  cout << page << endl;
};
void bittrex::cancel_orders() {
  auto &ords=load_orders();
  for( auto ord : ords )
    cancel_order(ord.uuid());
}
bool bittrex::orders_pending() {
  auto &ords=load_orders();
  return !!ords.size();
};
void bittrex::dump_orders() {
  auto &ords=load_orders();
  for( auto &ord : ords )
  {
    cout << json_str(ord) << endl;
  };
};
const order_l bittrex::load_orders() {
  order_l res;
  const static string url("https://bittrex.com/api/v1.1/market/getopenorders?");
  json page = load_json(url,"log/openorders.json");
  bittrex::from_json(page,res);
  return res;
};

order_l bittrex::get_order( const string& uuid )
{
  const static string url( api_url + "account/getorder?" );
  string my_url=url+"uuid="+uuid+"&";
  if ( show_urls )
    xexpose(my_url);
  xassert(uuid.size());
  string page = web::load_sec_page( my_url ,"");
  auto jpage = json::parse( page );
  save_json("log/order.json",jpage);
  jpage = jpage[ "result" ];
  order_l orders;
  order_t order;
  coin::from_json( jpage, order );
  orders.push_back( order );
  return orders;
};
order_l bittrex::get_order_history( const string& msg )
{
  const static string url( api_url + "account/getorderhistory?" );
  if ( show_urls )
  {
    xexpose(url);
  };
  auto jpage=load_json(url,"log/orders.json");
  order_l orders;
  for ( auto b( begin( jpage ) ), e( end( jpage ) ); b != e; b++ )
  {
    order_t order;
    coin::from_json( *b, order );
    orders.push_back( order );
  };
  return orders;
};
