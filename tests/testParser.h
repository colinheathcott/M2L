#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include "test.h"
#define TESTS \
    X(Call)

#define X(name) int Test##name();
TESTS
#undef X

void RunParserTests();

#endif
