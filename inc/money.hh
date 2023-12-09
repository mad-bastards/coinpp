#ifndef money_hh
#define money_hh money_hh

#include <fmt.hh>

namespace coin {
	class money_t : public fmt::streamable_tag
  {
		double val;
		public:
			money_t(double val=0)
				: val(val)
			{
			};
			double get() const
			{
				return val;
			};
			bool operator!() const
			{
				return !val;
			};
			explicit operator bool() const
			{
				return !!*this;
			};
      explicit operator double() const
      {
        return get();
      };
			money_t operator-() const
			{
				return -val;
			};
#define def_op(op) \
			money_t &operator op(money_t rhs) \
			{ \
				val op rhs.val; \
				return *this; \
			};
			def_op(+=); def_op(-=); def_op(*=); def_op(/=);
#undef def_op
#define def_op(op) \
			money_t operator op(money_t rhs) const \
			{ \
				return money_t(*this) op##= rhs; \
			};
			def_op(+); def_op(-); def_op(*);
#undef def_op
			double cmp(money_t lhs) const
			{
				return val-lhs.val;
			};
			int get_width() const;
			ostream &stream(ostream &lhs,int ind=0) const;
	};
#define def_op(op) \
			inline money_t operator op(double lhs, const money_t &rhs) \
			{ \
				return money_t(lhs) op##= rhs; \
			};
			def_op(+); def_op(-); def_op(*);
#undef def_op
			
      inline money_t operator/(money_t lhs, double rhs)
			{
				return money_t(lhs)/=rhs;
			};

			inline double operator/(money_t lhs, money_t rhs)
			{
				return lhs.get()/rhs.get();
			};

#define def_op(op) \
			inline bool operator op(money_t lhs, money_t rhs) \
			{ \
				return lhs.cmp(rhs) op 0.0; \
			};
			def_op(==); def_op(<); def_op(>); def_op(<=); def_op(>=);
			def_op(!=);
      
      inline money_t abs( money_t rhs )
      {
        return ( rhs < 0 ) ? -rhs : rhs;
      };

};

#endif

