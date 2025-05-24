#ifndef XLOG_H
#define XLOG_H

#include <stdint.h>
#include <time.h>

#define SYSLOG_ADDRESS "192.168.0.27"
#define SYSLOG_PORT 10514
#define SYSLOG_MSG_MAX 1024
#define SYSLOG_FACILITY (1 << 3) // USER facility
#define SYSLOG_EMERG 0
#define SYSLOG_ALERT 1
#define SYSLOG_CRIT 2
#define SYSLOG_ERR 3
#define SYSLOG_WARNING 4
#define SYSLOG_NOTICE 5
#define SYSLOG_INFO 6
#define SYSLOG_DEBUG 7

void xlog_init(const char *remote_host, uint16_t port);
time_t xlog_start(const char *fmt, ...);
void xlog_info(const char *fmt, ...);
void xlog_error(const char *fmt, ...);
void xlog_fatal(const char *fmt, ...);
void xlog_panic(const char *fmt, ...);
void xlog_stop(time_t start, const char *fmt, ...);
void xlog_close();

#endif // XLOG_H

