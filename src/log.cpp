#include "sipster/log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

int logLevel = -1;

#define PRINTF_LOG(level, levelStr, output) if(level <= logLevel) printf("%s: %s\n", levelStr, output)

void sipster_log_set_level_from_env() {
    const char * lvl = getenv("SIPSTER_LOG_LEVEL");
    if(!lvl) {
        logLevel = 0;
        return;
    }
    logLevel = atoi(lvl);
}


void sipster_log(SipsterLogLevel level, const char * format, ...) {

    char output[1000];

    if(logLevel < 0) {
        sipster_log_set_level_from_env();
    }

    va_list args;
    va_start( args, format );

    vsprintf(output, format, args);

    switch(level) {
        case LOG_TRACE:
            PRINTF_LOG(LOG_TRACE, "TRACE", output);
        break;
        case LOG_DEBUG:
            PRINTF_LOG(LOG_DEBUG, "DEBUG", output);
        break;
        case LOG_INFO:
            PRINTF_LOG(LOG_INFO, "INFO", output);
        break;
        case LOG_WARN:
            PRINTF_LOG(LOG_WARN, "WARN", output);
        break;
        case LOG_ERROR:
            PRINTF_LOG(LOG_ERROR, "ERROR", output);
        break;
    }
    va_end( args );
}

