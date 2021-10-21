#include <stdio.h>

#include "gtest/gtest.h"

// --gtest_filter=-*2_GPU_MultiGPU:*Gauss*

//GTEST_API_ int main(int argc, char **argv) {
//  testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
//}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

