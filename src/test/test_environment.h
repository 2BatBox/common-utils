#pragma once

#include <cstdio>
#include <cassert>
#include <typeinfo>

#include "utils/DiceMachine.h"

#ifndef TEST_TRACE
#define TEST_TRACE {printf("-> %s::%s()\n", typeid(*this).name(), __FUNCTION__);}
#endif // TEST_TRACE
