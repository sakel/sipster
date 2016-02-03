#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace std;

string nextToken(string input, string delimiter, std::size_t *pos);
string getRest(string input, std::size_t *pos);

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

void * sipster_allocator(size_t size);
void sipster_free(void * ptr);
char * sipster_generate_uuid();
char * sipster_generate_random_string(size_t len);
#endif // UTILS_H
