/**
	Downsampling assignment 

	test2.cpp
	Author: Alexey Imaev, 2014
*/

#include <cstdlib>
#include "downsampling.h"

/**
	Test harness
*/
void test2() {
	int num_dims = 2;
	int d1 = 32;
	int d2 = 8;

	UintArray2d A(boost::extents[d1][d2]);

	/// Assign values to the elements
	unsigned int values = 0;
	for(index i = 0; i != A.shape()[0]; ++i) {
		for(index j = 0; j != A.shape()[1]; ++j) {
			A[i][j] = rand()%3;
		}
	}

	/// Verify values
	printArray(A);

	std::vector<UintArray2d> results;
	computeDownsamplesParallel(A, results);

	for (int i = 0; i != results.size();  ++i) {
		printArray(results[i]);
	}
	
	//results.clear();
	//computeDownsamples(A, results);
	
	for (int i = 0; i != results.size();  ++i) {
		printArray(results[i]);
	}
}
