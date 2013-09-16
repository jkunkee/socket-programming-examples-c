#include "logger.h"

#include <cstdio>
#include <cstdarg>

Logger* Logger::getInstance() {
    static Logger instance;
    return &instance;
}

Logger::Logger() {
    do_log = true;
}
Logger::~Logger() {}

void Logger::doLog() {
    do_log = true;
}
void Logger::doNotLog() {
    do_log = false;
}
void Logger::log(const char *fmt, ...) {
    if (do_log) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
    }
}
