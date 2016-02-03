#ifndef LOG_H
#define LOG_H

typedef enum {
    LOG_ERROR = 0,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE
} SipsterLogLevel;

#define SIPSTER_SIP_TRACE(f, ...) (sipster_log("SIP", LOG_TRACE, f, ##__VA_ARGS__))
#define SIPSTER_SIP_DEBUG(f, ...) (sipster_log("SIP", LOG_DEBUG, f, ##__VA_ARGS__))
#define SIPSTER_SIP_INFO(f, ...) (sipster_log("SIP", LOG_INFO, f, ##__VA_ARGS__))
#define SIPSTER_SIP_WARN(f, ...) (sipster_log("SIP", LOG_WARN, f, ##__VA_ARGS__))
#define SIPSTER_SIP_ERROR(f, ...) (sipster_log("SIP", LOG_ERROR, f, ##__VA_ARGS__))

#define SIPSTER_TRACE(f, ...) (sipster_log("SIPSTER", LOG_TRACE, f, ##__VA_ARGS__))
#define SIPSTER_DEBUG(f, ...) (sipster_log("SIPSTER", LOG_DEBUG, f, ##__VA_ARGS__))
#define SIPSTER_INFO(f, ...) (sipster_log("SIPSTER", LOG_INFO, f, ##__VA_ARGS__))
#define SIPSTER_WARN(f, ...) (sipster_log("SIPSTER", LOG_WARN, f, ##__VA_ARGS__))
#define SIPSTER_ERROR(f, ...) (sipster_log("SIPSTER", LOG_ERROR, f, ##__VA_ARGS__))

/**
 * Logging facade for sipster allowing the use of any logging backend
 *
 * @brief sipster_log
 * @param logLevel
 * @param format
 */
void sipster_log(const char * ns, SipsterLogLevel logLevel, const char * format, ...);

#endif // LOG_H

