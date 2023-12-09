#ifndef balance_hh
#define balance_hh balance_hh
#include <money.hh>
#include <symbol.hh>

namespace coin {
  class balance_l;
	struct balance_t : public fmt::streamable_tag {
		sym_t sym;
		money_t bal;
		money_t ava;
		money_t pend;
		money_t btc;
    string addr;
		bool operator<(const balance_t &rhs) const;
    bool operator>(const balance_t &rhs) const;
		ostream &stream(ostream &lhs, int ind=0) const;
		~balance_t();
    static const balance_t &get(const sym_t &sym, bool except=true);
		static const balance_l &get_balances();
		static const balance_l &load_balances();
    static balance_l list;
	};
	struct balance_l : public std::vector<balance_t> {
		balance_l()
		{
		};
		template<typename itr_t>
			balance_l(itr_t b, itr_t e)
			: std::vector<balance_t>(b,e)
			{
			};
		static const balance_l &load_balances();
		const balance_t &get(const string &sym) const {
			for( auto &b : *this ) {
				if(b.sym==sym)
					return b;
			};
			throw runtime_error("no such bal!");
		};
		sym_l syms() const;
	};
};
#endif

