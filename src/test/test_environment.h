#ifndef QLIBS_TEST_ENVIRONMENT_H
#define QLIBS_TEST_ENVIRONMENT_H

#include <stdio.h>
#include <assert.h>

#ifndef TRACE_CALL
#define TRACE_CALL_CPP {printf("-> %s::%s()\n", typeid(*this).name(), __FUNCTION__);}
#endif // TRACE_CALL

#endif //QLIBS_TEST_ENVIRONMENT_H
