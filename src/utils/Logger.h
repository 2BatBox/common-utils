#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

/*
 * size_t x = ...;
 * ssize_t y = ...;
 * printf("%zu\n", x);  // prints as unsigned decimal
 * printf("%zx\n", x);  // prints as hex
 * printf("%zd\n", y);  // prints as signed decimal 
 * 
 **/

#define LOGC(...) {fprintf(Logger::log_file, __VA_ARGS__);}
#define LOGNL {fprintf(Logger::log_file, "\n"); fflush(Logger::log_file);}

#define LOG_RAW(...) {fprintf(Logger::log_file, __VA_ARGS__); fprintf(Logger::log_file, "\n");fflush(Logger::log_file);}

#define LOG_INFO(...) { \
    fprintf(Logger::log_file, "[info] %s() %s:%d ", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(Logger::log_file, __VA_ARGS__);\
	fprintf(Logger::log_file, "\n");\
    fflush(Logger::log_file); \
}

#define LOG_CRITICAL(...) { \
    fprintf(Logger::log_file, "[critical] %s() %s:%d ", __FUNCTION__, __FILE__, __LINE__); \
    fprintf(Logger::log_file, __VA_ARGS__);\
	fprintf(Logger::log_file, "\n");\
    fflush(Logger::log_file); \
}

class Logger {
public:
    static FILE* log_file;
};

#endif /* LOG_H_ */
