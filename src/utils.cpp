#include <sipster/utils.h>
#include <uuid.h>
#include <netinet/in.h>

using namespace std;

string nextToken(string input, string delimiter, std::size_t *pos) {

    if(*pos >= input.length() || *pos == std::string::npos) {
        return "";
    }

    std::size_t new_pos = input.find_first_of(delimiter, *pos);
    string token = input.substr(*pos, new_pos-*pos);

    *pos = new_pos;
    if(new_pos != std::string::npos) {
        *pos = *pos + delimiter.size();
    }
    if(token.empty()) {
        return "";
    }

    return token;
}

string getRest(string input, std::size_t *pos) {

    if(*pos >= input.length() || *pos < 0) {
        return "";
    }

    string token = input.substr(*pos, input.length()-*pos);
    if(token.empty())
        return "";

    *pos = std::string::npos;

    return token;
}

void * sipster_allocator(size_t size) {
    return calloc(1, size);
}

void sipster_free(void * ptr) {
    if(ptr)
        free(ptr);
}

char * sipster_generate_uuid() {
    uuid_t * uuid;
    uuid_rc_t rc = uuid_create(&uuid);
    if(rc != 0) {
        return NULL;
    }

    rc = uuid_make(uuid, UUID_MAKE_V1);
    if(rc != 0) {
        return NULL;
    }

    char *output = NULL;
    size_t len = 0;
    rc = uuid_export(uuid, UUID_FMT_STR, &output, &len);
    if(rc != 0) {
        return NULL;
    }

    uuid_destroy(uuid);

    return output;
}

char * sipster_generate_random_string(size_t len) {
    int i;
    char * s = (char *) sipster_allocator(len+1);
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (i = 0; i < (int)len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;

    return s;
}
