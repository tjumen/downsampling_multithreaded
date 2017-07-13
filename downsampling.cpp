/**
	downsampling.h
	Author: Alexey Imaev, 2014
*/

#include "downsampling.h"

using namespace std;
using namespace tbb;


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
		and n_cores is the number of available processing cores on the system running the algorithm.
		Note that when A contains a lot values that are same, 
		the algorithm's actual run-time will be much less that its worst case run-time complexity.
*/
void computeDownsamplesParallel(const UintArray2d &A, vector<UintArray2d> &results) {

	/// number of dimentions 
	int ndims = A.dimensionality;

	/// size of the input array
	size_t d1 = A.shape()[0] / 2;
	size_t d2 = A.shape()[1] / 2;

	HashMapArray2d hashMapArray( boost::extents [d1] [d2] );
	UintArray2d outputArray( boost::extents [d1] [d2] );

	/** 
		Instantiate the Body object for tbb::parallel_for 
	*/
	ParallelCreateMaps parallelCreateMaps(A, &hashMapArray, &outputArray);

	/** 
		Parallel loop.
		Given an original array A it divides the array into blocks of size 2x2 
		(for the case of 2-dim array, it will be 2x2x2 subarray for 3-d array, etc.), 
		In each iteration of the loop it computes a hashmap for a 2x2 sub-array 
		where a hushmap contains the number of occurances of each element in the block. 
		It outputs hashMapArray (array of hashmaps) and outputArray (1-downsampled image).	
	*/
	parallel_for(tbb::blocked_range<size_t>(0,  d1*d2), parallelCreateMaps);

	// push 1-downsampled image into results
	results.push_back(outputArray);

	d1 = d1/2;
	d2 = d2/2;


	while ( min(d1, d2) >= 1) {	
		HashMapArray2d outputHashMapArray( boost::extents [d1] [d2] );
		outputArray.resize( boost::extents [d1] [d2] );

		/** 
			Instantiate the Body object for tbb::parallel_for 
		*/
		ParallelMergeMaps parallelMergeMaps(&hashMapArray, &outputHashMapArray, &outputArray);

		/** 
			Parallel loop.
			Given an array of hashmaps it divides the array into blocks of size 2x2.
			In each iteration of the loop the function merges all hash maps in a 2x2 block into a single hash map
			summing values for same keys. 
			It outputs outputHashMapArray (reduced array of hashmaps) and outputArray (downsampled image).
		*/
		parallel_for(tbb::blocked_range<size_t> (0, d1 * d2 ), parallelMergeMaps);


		hashMapArray.resize( boost::extents [d1] [d2] );
		hashMapArray = outputHashMapArray;
		results.push_back(outputArray);

		/// Note that now the size of the hashMapArray has been reduced by the factor of 2 along each dimention

		d1 = d1/2;
		d2 = d2/2;
	}
}


/**
    Single threaded version of void computeDownsamplesParallel(..) function

	Worst case time complexity: O(N * num_downsamples), where N is the total number of elements in original array A and
		nd = min(L1, L2 ..., Ld) is the total number of possible downsamplings 
*/
void computeDownsamples(const UintArray2d &A, vector<UintArray2d> &results) {
	
	int block_size = 2;
	UintArray2d::index_gen indices;

	/// size of the input array
	size_t d1 = A.shape()[0] / 2;
	size_t d2 = A.shape()[1] / 2;

	HashMapArray2d hashMapArray(boost::extents[d1][d2]);
	UintArray2d outputArray(boost::extents[d1][d2]);
	unsigned int mode;
	
	/// "flattened" version of the loop (uses getIndices() function)
	vector<size_t> res_index;
	std::vector<size_t> sizes;
	sizes.push_back(A.shape()[0]);
	sizes.push_back(A.shape()[1]);
	for (index i=0; i != d1*d2; ++i) {
		res_index = getIndices(i, sizes);
		size_t x = res_index[0];
		size_t y = res_index[1];
		UintArray2d::const_array_view<2>::type view = A[ indices[range(x, x + block_size)][range(y, y + block_size)] ];
		mode = createMap(view, hashMapArray[x/2][y/2]);
		outputArray[x/2][y/2] = mode;
	}

	/// nested version of the loop (iterates along each dimention of an array)
	/*
	for (index x=0, xx=0; x != A.shape()[0]; x = x + block_size, ++xx) {
		for (index y=0, yy=0; y != A.shape()[1]; y = y + block_size, ++yy) {
			UintArray2d::const_array_view<2>::type view = A[ indices[range(x, x + block_size)][range(y, y + block_size)] ];
			mode = createMap(view, hashMapArray[xx][yy]);
			outputArray[xx][yy] = mode;
		}
	}
	*/

	results.push_back(outputArray);

	d1 = d1/2;
	d2 = d2/2;

	while ( min(d1, d2) >= 1) {
		
		HashMapArray2d outputHashMapArray(boost::extents[d1][d2]);
		outputArray.resize( boost::extents [d1] [d2] );

		/// "flattened" version of the loop (uses getIndices() function)
		vector<size_t> res_index;
		sizes.clear();
		sizes.push_back(hashMapArray.shape()[0]);
		sizes.push_back(hashMapArray.shape()[1]);
		for (index i=0; i != d1*d2; ++i) {
			res_index = getIndices(i, sizes);
			size_t x = res_index[0];
			size_t y = res_index[1];
			HashMapArray2d::const_array_view<2>::type view = hashMapArray[ indices [range(x, x + block_size)][range(y, y + block_size)] ];
			mode = mergeMaps(view, outputHashMapArray[x/2][y/2]);
			outputArray[x/2][y/2] = mode;
		}

		/// nested version of the loop (iterates along each dimention of an array)
		/*
		for (index x=0, xx=0; x != hashMapArray.shape()[0]; x = x + block_size, ++xx) {
			for (index y=0, yy=0; y != hashMapArray.shape()[1]; y = y + block_size, ++yy) {
				HashMapArray2d::const_array_view<2>::type view = hashMapArray[ indices [range(x, x + block_size)][range(y, y + block_size)] ];
				mode = mergeMaps(view, outputHashMapArray[xx][yy]);
				outputArray[xx][yy] = mode;
			}
		}
		*/

		hashMapArray.resize(boost::extents[d1][d2]);
		hashMapArray = outputHashMapArray;
		results.push_back(outputArray);

		/// the size of the hashMapArray has been reduced by the factor of 2 along each dimention
		
		d1 = d1 / 2;
		d2 = d2 / 2;
	}
}

/**
	Constructor parameters: 
		A - original input array/image
		hash_array - output, array of hashmaps.
		result - output, 1-downsampled image
*/
ParallelCreateMaps::ParallelCreateMaps(const UintArray2d &A, HashMapArray2d *hash_array, UintArray2d *result)
	: A(A), sizes(sizes), hash_array(hash_array), result(result) {

		int ndims = A.dimensionality;
		for (int i = 0; i != ndims; ++i) {
			sizes.push_back(A.shape()[i]);
		}
}


/**
	operator() defines how we should process a chunk of the loop (by tbb::parallel_for)
	
	Given an original 2-d array A, it devides the array into blocks of size 2x2.
	Then it computes a hashmap for each 2x2 sub-array (it will be 2x2x2 subarray for 3-d array, etc), 
	where a hushmap contain counts corresponding to the number of different pixel values in
	the 2x2 subarray. It outputs array of hashmaps and 1-downsampled image.

	Note that instead of iterating along the each dimention of the array and having nested for loops as a result,
	we use a single for loop. The trick is to use "index_ve = getIndices(..)" function, 
	which takes a global index of the element and returns a vector of indices along 
	each dimention. It should be straightforward to modify this function to accept arrays of higher dimentions.
*/
void ParallelCreateMaps::operator()( const tbb::blocked_range<size_t>& r ) const {
	UintArray2d A_local = A;
	for( size_t i=r.begin(); i!=r.end(); ++i ) {

		// given single (global) array index i, we obtain array indeces (x,y) along each dimention of the array (takes O(1) time)
		std::vector<size_t> index_vec = getIndices(i, sizes);
		size_t x = index_vec[0];
		size_t y = index_vec[1];

		int block_size = 2;
		unsigned int mode;

		UintArray2d::const_array_view<2>::type view = A[ indices[range(x, x + block_size)][range(y, y + block_size)] ];
		mode = createMap(view, (*hash_array)[x/2][y/2]);
		(*result)[x/2][y/2] = mode;
	}		
}


/**
	Constructor parameters: 
		input_array - input, 2-d array of hash maps
		output_array - output, 2-d array of hash maps 
		result - output, downsampled image
*/
ParallelMergeMaps::ParallelMergeMaps(HashMapArray2d *input_array, HashMapArray2d *output_array, UintArray2d *result)
	: input_array(input_array), sizes(sizes), output_array(output_array), result(result) {

		int ndims = input_array->dimensionality;
		for (int i = 0; i != ndims; ++i) {
			sizes.push_back(input_array->shape()[i]);
		}
}


/**
	operator() defines how we should process a chunk of the loop.
	Given an array of hashmaps it devides the array into blocks of size 2x2.
	In each loop iteration the function merges all hash maps in a 2x2 block into a single hash map
	summing values for same keys. It outputs a reduced array of hashmaps and a downsampled image.

	Note that instead of iterating along the each dimention of the array and having nested for loops as a result,
	we use a single for loop. The trick is to use "index_ve = getIndices(..)" function, 
	which takes a global index of the element and returns a vector of indices along 
	each dimention. It should be straightforward to modify this function to accept arrays of higher dimentions.
*/
void ParallelMergeMaps::operator()( const tbb::blocked_range<size_t>& r ) const {
	for( size_t i=r.begin(); i!=r.end(); ++i ) {

		// given single (global) array index i, we obtain array indeces (x,y) along each dimention of the array (takes O(1) time)
		std::vector<size_t> index_vec = getIndices(i, sizes);
		size_t x = index_vec[0];
		size_t y = index_vec[1];

		int block_size = 2;
		unsigned int mode;

		HashMapArray2d::const_array_view<2>::type view = (*input_array)[ indices [range(x, x + block_size)][range(y, y + block_size)] ];
		mode = mergeMaps(view, (*output_array)[x/2][y/2]);
		(*result)[x/2][y/2] = mode;
	}		
}