#ifndef symbol_hh
#define symbol_hh symbol_hh

#include <coinfwd.hh>

namespace coin {
	using std::string;
	using std::ostream;
	class sym_t : public string 
	{
		using std::string::string;
		public:
		static int get_width() {
			return 8;
		};
    sym_t(const string &str)
    : string(str)
    {
    };
	};
	class sym_l : public vector<sym_t>
 	{
	};
	ostream &operator<<(ostream &lhs, const sym_t &rhs);
};


#endif

