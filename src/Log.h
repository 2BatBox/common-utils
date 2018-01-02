#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#define LOGC(...) {printf(__VA_ARGS__);}
#define LOG(...) {printf(__VA_ARGS__); printf("\n");}
#define LOGNL {printf("\n");}
#define LOGE(...) { \
	((void)printf("**** ERROR **** IN %s:%d\n", __FILE__, __LINE__)); \
	((void)printf(__VA_ARGS__));\
	((void)printf("\n"));\
}

#endif /* LOG_H_ */
