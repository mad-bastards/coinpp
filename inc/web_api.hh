#ifndef web_api_hh
#define web_api_hh web_api_hh


#include <coinfwd.hh>

#if 0
typedef vector<unsigned char> data_slice;
hash_digest hmac_sha256_hash(const data_slice& data, const data_slice& key);

std::string encode_base16(const data_slice& data);
#endif
namespace web {
	using std::list;


	const string load_sec_page(const string &url, const string &content);
//     inline const string load_sec_page(const string &url, const string &content) {
//       return load_sec_page(url,content,list<string>());
//     };
	const string load_page(const string &url, const list<string> headers);
//   	inline const string load_page(const string &url )
//   	{
//   		return load_page(url,list<string>());
//   	};
	const string load_hmac_page( const string &url );
	string pp_json_url(const string &url);
	string pp_json(const string &url);
	extern bool verbose;
};

#endif

