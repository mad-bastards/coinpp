#ifndef order_hh
#define order_hh order_hh

#include <coinfwd.hh>
#include <money.hh>
#include <fmt.hh>

namespace coin {
	class order_t : public fmt::streamable_tag
	{
    public:
    struct data_t {
      string   uuid;
      string   account_id;
      string   exchange;
      string   type;
      string   opened;
      string   closed;
      bool     is_open;

      money_t  limit;
      money_t  quantity;
      money_t  price;
      money_t  price_per_unit;
      money_t  quantity_remaining;

      bool     cancel_initiated;

      money_t  commission_paid;
      money_t  commission_reserved;
      money_t  commission_remain;

      bool     is_conditional;
      string   condition;
      money_t   condition_target;

      bool   immediate_or_cancel;
      string   reserved;
      string   sentinel;
    };
    order_t();
    order_t(const data_t &data);
    order_t(const order_t &rhs);
    order_t& operator=(const order_t &rhs);
    const string &uuid() const
    {
      return data.uuid;
    };
    bool is_open() const {
      return data.is_open;
    };
    ~order_t();
		ostream &stream(ostream &lhs, int ind=0) const;
    const data_t &get_data() const {
      return data;
    };
    data_t data;
	};
	class order_l : public std::vector<order_t>, public fmt::streamable_tag
  {
    using std::vector<order_t>::vector;
  public:
    ~order_l();
		ostream &stream(ostream &lhs, int ind=0) const;
  };
};

#endif

