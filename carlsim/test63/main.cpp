#include <stdio.h>

#include "gtest/gtest.h"


// --gtest_filter=ADK13.*
// --gtest_filter=*NrnMon*


//GTEST_API_ int main(int argc, char **argv) {
//  testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
//}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

