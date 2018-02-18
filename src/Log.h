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

#define LOGC(...) {printf(__VA_ARGS__);}
#define LOG(...) {printf(__VA_ARGS__); printf("\n");}
#define LOGNL {printf("\n");}
#define LOGE(...) { \
	((void)printf("**** ERROR **** IN %s:%d\n", __FILE__, __LINE__)); \
	((void)printf(__VA_ARGS__));\
	((void)printf("\n"));\
}

#endif /* LOG_H_ */
