#ifndef fmt_hh
#define fmt_hh fmt_hh

#include <coinfwd.hh>
#include <iostream>

namespace fmt {
  using std::ostream;
  using std::string;
  struct streamable_tag
  {
  };
#if 1
  template<typename rhs_t>
  inline typename std::enable_if<std::is_base_of<fmt::streamable_tag,rhs_t>::value,ostream &>::type &operator<<( ostream &lhs, const rhs_t &rhs )
  {
    return rhs.stream(lhs);
  };
#else
  template<typename rhs_t>
  inline typename std::enable_if<streamable_tag<rhs_t>::value,ostream &>::type &operator<<(ostream &lhs, const rhs_t &rhs)
  {
    return rhs.stream(lhs);
  };
#endif

  class fp_val : public streamable_tag
  {
    protected:
      double val;
      ~fp_val ();
      fp_val(double val)
        : val(val)
      {
      };
    public:
      string fmt() const;
      int get_width() const;
      double get() const {
        return val;
      };
      explicit operator double()const{
        return get();
      };
      bool operator!() const
      {
        return !val;
      };
  };
  class pct_t : public fp_val  {
    public:
      pct_t(double val=0)
        : fp_val(val)
      {
      };
      template<typename lhs_t, typename rhs_t>
        pct_t( lhs_t lhs, rhs_t rhs )
        : fp_val(lhs/rhs)
        {
        };
      int get_width() const;
      string fmt() const;
      pct_t &operator+=(const pct_t&rhs)
      {
        val+=rhs.val;
        return *this;
      };
      pct_t &operator-=(const pct_t&rhs)
      {
        val+=rhs.val;
        return *this;
      };
      operator bool() const {
        return !!val;
      };
      ostream &stream(ostream &lhs, int ind=0) const;
  };
  template<typename otype_t>
    inline otype_t operator*(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs*rhs.get();
    };
  template<typename otype_t>
    inline otype_t operator*(const pct_t &lhs, const otype_t &rhs)
    {
      return lhs.get()*rhs;
    };
  template<typename otype_t>
    pct_t operator+(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs+rhs.get();
    };
  template<typename otype_t>
    inline pct_t operator-(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs-rhs.get();
    };
  template<>
    inline pct_t operator-<pct_t>(const pct_t &lhs, const pct_t &rhs)
    {
      return lhs.get()-rhs.get();
    };
  template<typename otype_t>
    inline otype_t operator/(const otype_t &lhs, const pct_t &rhs)
    {
      return lhs*(1/rhs.get());
    };
  struct nl_t {
    friend ostream &operator<<(ostream &lhs, nl_t nl)
    {
      return lhs.put('\n');
    }
  };
  const static nl_t nl;
}
#endif

