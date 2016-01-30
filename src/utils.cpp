#include "utils.h"
#include <sipster/log.h>

using namespace std;

string nextToken(string input, string delimiter, std::size_t *pos) {

    if(*pos >= input.length() || *pos == std::string::npos) {
        return "";
    }

    std::size_t new_pos = input.find_first_of(delimiter, *pos);
    string token = input.substr(*pos, new_pos-*pos);

    *pos = new_pos;
    if(token.empty()) {
        return "";
    }
    if(new_pos != std::string::npos) {
        *pos = *pos + delimiter.size();
    }

    return token;
}

string getRest(string input, std::size_t *pos) {

    if(*pos >= input.length() || *pos < 0) {
        return "";
    }

    string token = input.substr(*pos, std::string::npos);
    if(token.empty())
        return "";

    *pos = std::string::npos;

    return token;
}

