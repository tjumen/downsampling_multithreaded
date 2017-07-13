/**
	Downsampling assignment

	utilities.h
	Author: Alexey Imaev, 2014
*/

#pragma once
#include "boost/multi_array.hpp"
#include <unordered_map>

// hashmap that that stores key-value pairs.  
typedef std::unordered_map<unsigned int, unsigned int> HashMap;

// 2 dimentional array of unsigned integers.
typedef boost::multi_array<unsigned int, 2> UintArray2d;

// 2 dimentional array of HashMap
typedef boost::multi_array<HashMap, 2> HashMapArray2d;

// index used to address a particular element in boost::multi_array
typedef boost::multi_array_types::index index;

// range is used to specify the range of indices
typedef boost::multi_array_types::index_range range;


/**
    The function counts how many times each element occurs in array A.
	Parameters:	
		A – Input array
		hashmap - Output hashmap
	Given array A the function returns hashmap. 
	Hashmap's keys are elements of A. Each key contains a `value` 
	which is equal to the number of occurances of that key in A. 

	Complexity: Max number of elements in A is 2^ndims, where ndims is the number of dimentions.
		Therefore, the function has O(1), where N is the total number of elements in the original array
*/
unsigned int createMap(const UintArray2d &A, HashMap & hashmap);


/**
	The function takes an array of hashmaps and merges them together into a single hashmap 
	summing values for same keys. 

	Parameters:	
		array_of_maps – Input array of hashmaps
		output_map - Output hashmap	

	Complexity: O(N) where N is the number of elements in output_map
*/
unsigned int mergeMaps(HashMapArray2d array_of_maps, HashMap & output_map);


/**
	The takes a linear array index (single number) and array dimentions.
	It returns the n-dimentional index into array.
	Parameters:
		input - input linear (1 dimentional) index
		sizes - size of array along all n dimentions
	Returns: 
		vector containing n-dimentional index into array
	Complexity: O(1)
*/
std::vector<size_t> getIndices(size_t input, std::vector<size_t> sizes);


/**
	Prints 2 dimentional array of unsigned integers
*/
void printArray(const UintArray2d &A);


/**
	Prints hash map
*/
void printHashMap(HashMap map);