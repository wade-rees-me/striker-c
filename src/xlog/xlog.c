#include "xlog.h"
#include "../constants/constants.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int syslog_socket = -1;
static struct sockaddr_in syslog_server;
static char hostname[64] = "blackjack";

// Initialize the syslog connection
void xlog_init(const char *remote_host, uint16_t port) {
    syslog_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (syslog_socket < 0) {
        perror("socket");
        return;
    }

    memset(&syslog_server, 0, sizeof(syslog_server));
    syslog_server.sin_family = AF_INET;
    syslog_server.sin_port = htons(port);
    if (inet_pton(AF_INET, remote_host, &syslog_server.sin_addr) <= 0) {
        perror("inet_pton");
        return;
    }

    gethostname(hostname, sizeof(hostname));
}

// Internal log function
void xlog_syslog_str(int severity, const char *message) {
    if (syslog_socket < 0) {
        return;
    }

    char packet[SYSLOG_MSG_MAX];
    int priority = SYSLOG_FACILITY + severity;

    snprintf(packet, sizeof(packet), "<%d>%s: [version=%s] [PID=%d] | %s", priority, STRIKER_WHO_AM_I, STRIKER_VERSION,
             getpid(), message);

    sendto(syslog_socket, packet, strlen(packet), 0, (struct sockaddr *)&syslog_server, sizeof(syslog_server));
}

void xlog_syslog(int severity, const char *fmt, va_list args) {
    if (syslog_socket < 0) {
        return;
    }

    char message[SYSLOG_MSG_MAX];
    vsnprintf(message, sizeof(message), fmt, args);
    xlog_syslog_str(severity, message);
}

// Convenience wrappers
time_t xlog_start(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xlog_syslog(SYSLOG_INFO, fmt, args);
    va_end(args);
    return time(NULL);
}

void xlog_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xlog_syslog(SYSLOG_INFO, fmt, args);
    va_end(args);
}

void xlog_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xlog_syslog(SYSLOG_ERR, fmt, args);
    va_end(args);
}

void xlog_fatal(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xlog_syslog(SYSLOG_CRIT, fmt, args);
    va_end(args);
}

void xlog_panic(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    xlog_syslog(SYSLOG_CRIT, fmt, args);
    va_end(args);
    exit(0);
}

void xlog_stop(time_t start_time, const char *fmt, ...) {
    time_t now = time(NULL);
    if (now == ((time_t)-1)) {
        perror("time");
        return;
    }

    time_t elapsed = now - start_time;
    if (elapsed < 0) {
        elapsed = 0; // Protect against time anomalies
    }

    // Format the user-provided message
    char user_msg[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(user_msg, sizeof(user_msg), fmt, args);
    va_end(args);

    // Format the final message with elapsed time
    char final_msg[1024];
    snprintf(final_msg, sizeof(final_msg), "%s | total time = %lds", user_msg, elapsed);

    // Send the log
    xlog_syslog_str(SYSLOG_INFO, final_msg);
}

// Close the socket when done
void xlog_close() {
    if (syslog_socket >= 0) {
        close(syslog_socket);
    }
}

