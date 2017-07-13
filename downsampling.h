/**
	Downsampling assignment

	downsampling.h
	Author: Alexey Imaev, 2014
*/

#pragma once

#include "boost/multi_array.hpp"
#include <cassert>
#include <unordered_map>
#include <iostream>
#include "tbb/parallel_for.h"
#include "utilities.h"


/**
    The function computes block downsampling of an original multidimentional array using modal values.
	Parameters:	
		A – a 2-dimensional array of size 2^L1 by 2^L2.
		results - Output vector contains all l-downsamplings of the original image.
	More information about downsampling can be found in the description of the assignment

	The function is parallelized to take advantage of multiple processor cores.

	In order to keep the solution unclattered and better demonstrate the idea (and also because of shortage of time)
	this function accepts only 2-dimentional arrays as its input.
	Note however, the code can easily be modified to address inputs of other dimentions. 

	Time complexity: O(N * num_downsamples / n_cores), where N is the total number of elements in original array A,
		nd = min(L1, L2 ..., Ld) is the total number of possible downsamplings 
		and n_cores is the number of available processing cores on the system running the algorithm
		Note that when A contains a lot values that are same, 
		the algorithm's actual run-time will be much less that its worst case run-time complexity.
*/
void computeDownsamplesParallel(const UintArray2d &A, std::vector<UintArray2d> &results);


/**
    Single threaded version of void computeDownsamplesParallel(..) function

	Worst time complexity: O(N * num_downsamples), where N is the total number of elements in original array A and
		nd = min(L1, L2 ..., Ld) is the total number of possible downsamplings 
*/
void computeDownsamples(const UintArray2d &A, std::vector<UintArray2d> &results);


/**
    ParallelCreateMaps class defines Body for TBB parallel_for in which operator() processes a chunk of the loop.
*/
class ParallelCreateMaps {  
	const UintArray2d A;
	UintArray2d::index_gen indices;
	HashMapArray2d *hash_array;
	UintArray2d *result;
	std::vector<size_t> sizes;   // size of the original image A

public:

	/**
		Constructor parameters: 
			A - original input array/image
			hash_array - output, array of hashmaps.
			result - output, 1-downsampled image
	*/
	ParallelCreateMaps(const UintArray2d &A, HashMapArray2d *hash_array, UintArray2d *result);


	/**
		operator() defines how we should process a chunk of the loop.
		Given an original 2-d array A, it devides the array into blocks of size 2x2.
		In a sinlge iteration of the loop it computes a hashmap for a 2x2 sub-array (or 2x2x2 subarray for 3-d array, etc.), 
		where a hushmap contain counts corresponding to the number of different pixel values in
		the sub-array. It outputs array of hashmaps and 1-downsampled image.
	*/
	void operator()( const tbb::blocked_range<size_t>& r ) const;
};


/**
    ParallelMergeMaps class defines Body for TBB parallel_for 
*/
class ParallelMergeMaps {
	UintArray2d::index_gen indices;
	HashMapArray2d *input_array;
	HashMapArray2d *output_array;
	UintArray2d *result;
	std::vector<size_t> sizes; // size of the *input_array

public:
	/**
		Constructor parameters: 
			input_array - input 2-d array of hash maps
			output_array - output 2-d array of hash maps 
			result - output, downsampled image
	*/
	ParallelMergeMaps(HashMapArray2d *input_array, HashMapArray2d *output_array, UintArray2d *result);


	/**
		operator() defines how we should process a chunk of the loop.
		Given an array of hashmaps it devides the array into blocks of size 2x2.
		In each iteration of the loop the function merges all hash maps in a 2x2 block into a single hash map
		summing values for same keys. It outputs a reduced array of hashmaps and a downsampled image.
	*/
	void operator()( const tbb::blocked_range<size_t>& r ) const; 
};