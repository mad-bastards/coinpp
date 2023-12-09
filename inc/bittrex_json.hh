#ifndef bittrex_json_hh
#define bittrex_json_hh bittrex_json_hh

#include <coinfwd.hh>
#include <json.hh>
#include <bittrex.hh>

namespace bittrex {
  void save_json(const string &fname, const json &json, bool backup=true);
  const json load_json(const string &url, const string &save_to, bool backup=true);
};
namespace fmt {
  class pct_t;
  void from_json(const json &j,       pct_t &todo);
  void to_json(      json &j, const pct_t &todo);
};
namespace coin {
  class balance_t;
  class todo_t;
  void from_json(const json &j,       balance_t &val);
  void from_json(const json &j,       money_t &val);
  void from_json(const json &j,       market_l &ml);
  void from_json(const json &j,       market_t &ml);
  void from_json(const json &j,       order_t& val );
  void from_json(const json &j,       order_l& val );
  void from_json(const json &j,       string &val);
  void from_json(const json &j,       bool &val);
  void from_json(const json &j,       todo_t &todo);
};
namespace coin {
void  to_json  (  json  &j,  const  balance_t  &val   );
void  to_json  (  json  &j,  const  money_t    &val   );
void  to_json  (  json  &j,  const  market_l   &ml    );
void  to_json  (  json  &j,  const  market_t   &ml    );
void  to_json  (  json  &j,  const  order_t&   val    );
void  to_json  (  json  &j,  const  order_l&   val    );
void  to_json  (  json  &j,  const  string     &val   );
void  to_json  (  json  &j,  const  bool       &val   );
void  to_json  (  json  &j,  const  todo_t     &todo  );
};

#endif
