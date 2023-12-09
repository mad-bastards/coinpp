#include <balance.hh>
#include <markets.hh>
#include <fmt.hh>
#include <bittrex.hh>

using fmt::nl;

namespace coin {
	balance_l balance_t::list;
	bool balance_t::operator>(const balance_t &rhs) const {
		if( btc > rhs.btc )
			return true;
		if( btc < rhs.btc )
			return false;
		return sym > rhs.sym;
	};
	bool balance_t::operator<(const balance_t &rhs) const {
		if( btc < rhs.btc )
			return true;
		if( btc > rhs.btc )
			return false;
		return sym < rhs.sym;
	};
	const balance_l &balance_l::load_balances()
	{
		return balance_t::load_balances();
	};
  const balance_t &balance_t::get(const sym_t &sym, bool except)
  {
    for( auto &bal : list )
    {
      if(bal.sym == sym)
        return bal;
    };
    if(except)
      throw runtime_error("unable to find balance for "+sym);
    static balance_t temp;
    temp.sym=sym;
    return temp;
  };
	const balance_l &balance_t::get_balances()
  {
    if(!list.size())
      load_balances();
    return list;
  };
	const balance_l &balance_t::load_balances()
	{
		list=bittrex::load_balances();
    cout << list.size() << " balances loaded" << endl;
    return list;
	};
	sym_l balance_l::syms() const
	{
		sym_l res;
		for( auto b : *this ) {
			res.push_back(b.sym);
		};
		return res;
	};
	balance_t::~balance_t() {
	};
	ostream &balance_t::stream(ostream &lhs, int ind) const {
		lhs
      << " | " << left << setw(8) << sym
      << " | " << right << setw(14) << bal
      << " | " << setw(14) << " BTC " << btc
      << " | " << setw(14) << ava << " |"
      << " | " << setw(14) << pend << " |"
      ;
		return lhs;
	};
};

