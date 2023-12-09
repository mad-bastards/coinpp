#include <money.hh>

int coin::money_t::get_width() const {
	return 17;
};
ostream &coin::money_t::stream(ostream&lhs, int ind) const
{
//   	char buf[128];
//   	snprintf(buf,sizeof(buf),"%16.8f ",get());
  ostringstream fmt;
  fmt
    << setprecision(8) << fixed
    << get();
	return lhs<<fmt.str();
}


