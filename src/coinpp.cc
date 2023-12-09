#include <json.hh>
#include <iostream>
#include <iomanip> 
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <symbol.hh>

using namespace std;

extern "C" {
  int main(int argc, char**argv, char**envp) ;
};
template<typename src_t>
struct range_t {
  typedef typename src_t::iterator iterator;
  typedef typename src_t::const_iterator const_iterator;
  typedef typename src_t::value_type value_type;
  typedef typename src_t::reference reference;
  typedef typename src_t::const_reference const_reference;
  iterator begin;
  iterator end;
  template<typename itr_t>
    range_t(const itr_t &begin, const itr_t &end)
    : begin(begin), end(end)
  {
  }
  range_t(src_t &src)
    : begin(src.begin()), end(src.end())
  {
  }
  range_t &operator++(int){
    if(begin!=end)
      ++begin;
    return *this;
  };
  range_t &operator*() const {
    return *this;
  };
  const range_t &operator*() {
    return *this;
  };
  operator bool() const {
    return begin<end;
  };
  void operator=(const string &str){
    assert (b!=end);
    *begin=str;
  }
};
template<typename src_t>
range_t<src_t> range(src_t src)
{
  return range_t<src_t>(src);
};
template<typename vec_t>
int xmain(const vec_t &args, const vec_t &envs){
  cout << "Hello, World!" << endl;
  cout << "ARGS: " << endl;
  for(auto r(range(args)); r; r++){
    cout << *r << endl;
  };
  cout << "ENVP: " << endl;
  for(auto r(range(envs)); r; r++){
    cout << *r << endl;
  };
  return 0;
};
int main(int argc, char**argv, char**envp) {
  char **arge(argv);
  while(*arge)
    ++arge;
  char **enve(envp);
  while(*enve)
    ++enve;
  vector<string> arg = vector<string>(argv,arge);
  vector<string> env = vector<string>(envp,enve);
  return xmain(arg,env);
}
