#include "TestCharClassifier.h"
#include "TestStreamTokenizer.h"
#include "TestStringTokenizer.h"
#include "TestTypes.h"
#include "TestBitArrayT.h"
#include "TestBitArray.h"
#include "TestBitStream.h"
#include "TestRangeBuffer.h"

#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {

	constexpr size_t capacity = 8 * 1024;

	TestCharClassifier test_char_classifier;
	TestStreamTokenizer test_stream_tokenizer;
	TestStringTokenizer test_string_tokenizer;
	TestTypes test_types;

	TestBitArrayT test_bit_arrayt(capacity);

	TestBitArray test_bit_array(capacity);

	TestBitStream test_bit_stream(capacity);

	TestRangeBuffer test_range_buffer(capacity);

	printf("<---- the end of main() ---->\n");
	return EXIT_SUCCESS;
}
