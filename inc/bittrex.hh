#ifndef bittrex_hh
#define bittrex_hh bittrex_hh
#include <coinfwd.hh>
#include <markets.hh>
#include <balance.hh>
#include <order.hh>

namespace bittrex {
	using namespace coin;
	const market_l load_markets();
	const balance_l load_balances();
	const order_l load_orders();

	extern bool fake_buys;
	extern bool show_urls;
  extern bool keep_all;
  extern bool fake_loads;

  order_l get_order_history(const string &mkt);
  order_l get_order( const string& uuid );
	void dump_orders();	
	void cancel_order(const string &id);	
  bool orders_pending();
	void cancel_orders();	
  bool orders_pending();
	void show_deposits();
	void show_withdrawals();
	const market_l load_markets();
	const balance_l load_balances();

  bool is_trading_pair( sym_t lhs, sym_t rhs );
  string simple_xact
    (
     const market_t &market,
     bool buy,    // buy or sell
     money_t qty, // in the symbol to buy or sell
     money_t rate, // in currency per unit
     bool ioc
    );
  string json_str(const coin::order_t &ord);
};

#endif // bittrex_hh


