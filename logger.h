#ifndef LOGGER_H
#define LOGGER_H

class Logger
{
public:
    // singleton
    static Logger* getInstance();
    ~Logger();

    void log(const char* fmt, ...);
    void doLog();
    void doNotLog();
private:
    // singleton
    Logger();

    bool do_log;
};

#define LOG Logger::getInstance()->log

#endif // LOGGER_H
