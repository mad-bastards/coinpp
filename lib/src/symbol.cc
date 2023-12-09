#include <symbol.hh>
#include <coinfwd.hh>


namespace coin {
  ostream &operator<<(ostream &lhs, const sym_t &rhs) {
    return lhs << (const string &)rhs;
  };
};

