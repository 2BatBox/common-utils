#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cassert>
#include <list>
#include <string>

// The 'builtin_clzll()' implementation depends on the GCC version currently used,
// it may use either BSF/BSR or LZCNT/TZCNT instructions.
// That's why X is needed to be checked for zero value to provide the same results regardless implementation difference.
#define LZCNTLL(X) (unsigned((X) ? __builtin_clzll(X) : (sizeof(X) << 3ull)))
#define TZCNTLL(X) (unsigned((X) ? __builtin_ctzll(X) : (sizeof(X) << 3ull)))



#include <cstdio>
#include <cassert>

#ifndef SB_CALL_TRACE
#define SB_CALL_TRACE { printf("-> %s()\n", __FUNCTION__);};
#endif // TRACE_TEST

#ifndef SB_CLASS_TRACE
#define SB_CLASS_TRACE(CLASS)  { printf("[%s]\n", CLASS);};
#endif // SB_CLASS_TRACE

#ifndef SB_ASSERT
#define SB_ASSERT(X) { assert(X); }
#endif // SB_ASSERT


int main(int argc, char** argv) {
	TestLinearAligner tla;

	printf("<---- the end of main() ---->\n");
	return EXIT_SUCCESS;
}
