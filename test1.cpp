/**
	downsampling.h
	Author: Alexey Imaev, 2014
*/

#include "downsampling.h"

/**
	Test harness
*/
void test1() {
	int num_dims = 2;
	int d1 = 64;
	int d2 = 16;

	UintArray2d A(boost::extents[d1][d2]);

	/// Assign values to the elements
	unsigned int values = 0;
	for(index i = 0; i != A.shape()[0]; ++i) {
		for(index j = 0; j != A.shape()[1]; ++j) {
			A[i][j] = (unsigned int)(i+j);
		}
	}

	/// Verify values
	printArray(A);

	std::vector<UintArray2d> results;
	computeDownsamplesParallel(A, results);
	//computeDownsamples(A, results);
	
	for (int i = 0; i != results.size();  ++i) {
		printArray(results[i]);
	}
}
