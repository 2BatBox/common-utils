#pragma once

#include <cstdio>
#include <cassert>

#ifndef TRACE_CALL
#define TRACE_CALL {printf("-> %s::%s()\n", typeid(*this).name(), __FUNCTION__);}
#endif // TRACE_CALL
