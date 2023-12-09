#ifndef markets_hh
#define markets_hh markets_hh

#include <money.hh>
#include <symbol.hh>
#include <fmt.hh>

namespace coin {
  using namespace std;
  class market_l;
  struct market_t : public fmt::streamable_tag 
  {
    struct data_t {
      string name;
      sym_t cur;
      sym_t sym;
      money_t ask;
      money_t bid;
      money_t high;
      money_t last;
      money_t prev;
      money_t low;
      money_t vol;
      money_t avg;
    } data;
    static market_l markets;
    market_t();
    market_t(const data_t &);
    market_t(const market_t &);
    public:
    market_t(const string &name, money_t bid=money_t(0), money_t ask=money_t(0));
    market_t reverse() const;
   
    explicit operator bool() const; 
    static bool split_name(const string &name, string &cur, string &t_coin);
    bool operator<(const market_t &rhs) const;
    const sym_t &cur()const{
      return data.cur;
    }
    const string &name()const{
      return data.name;
    }
    const sym_t sym()const{
      return data.sym;
    }
    money_t yield(money_t qty, sym_t f, sym_t t, bool neutral=false);
    const money_t & ask()const {
      return data.ask;
    };
    const money_t & bid()const {
      return data.bid;
    };
    const money_t &last()const {
      return data.last;
    };
    const money_t &high() const {
      return data.high;
    };
    const money_t &low() const {
      return data.low;
    };
    money_t avg()const {
      return (bid()+ask())/2;
    };
    ostream &stream(ostream &lhs, int ind=0) const;
    static money_t conv(money_t, const sym_t &from, const sym_t &to);
    static money_t conv2(money_t, const sym_t &from, const sym_t &to);
    static const market_l &get_markets( );
    static const market_l &load_markets();
    static market_l get(const string &from, const string &to, bool exceptions=true);
  };
  struct market_l : public vector<market_t>, public fmt::streamable_tag
  {
    ostream &stream(ostream &lhs, int ind=0) const;
  };
}
#endif






























