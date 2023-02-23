#ifndef _BASE64_
#define _BASE64_

#include <vector>
#include <string>
typedef unsigned char BYTE;

std::string base64_encode(BYTE const* buf, unsigned int buf_len);
std::vector<BYTE> base64_decode(std::string const&);

#endif