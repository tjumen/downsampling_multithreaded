/**
	Downsampling assignment 

	utilities.cpp
	Author: Alexey Imaev, 2014
*/

#include "utilities.h"
#include <iostream>

using namespace std;

/**
    The function counts how many times each element occurs in array A.
	Parameters:	
		A – Input array
		hashmap - Output hashmap
	Given array A, the function returns hashmap. 
	Hashmap's keys are elements of A. Each key contains a `value` 
	which is equal to the number of occurances of that key in A. 
*/
unsigned int createMap(const UintArray2d &A, HashMap &hashmap) {

	HashMap::iterator it;
	pair<unsigned int, unsigned int> array_index(0,0);
	
	unsigned int max_num_occurances = 1;
	for (int i=0; i != A.shape()[0]; ++i) {

		for (int j=0; j != A.shape()[1]; ++j) {
			it = hashmap.find(A[i][j]);
			if( it == hashmap.end()) {
				hashmap.insert(std::pair<unsigned int, unsigned int>(A[i][j],1));
			}
			else {
				it->second = it->second + 1;
				
				if (max_num_occurances < it->second) {
					max_num_occurances = it->second;
					array_index.first = i;
					array_index.second = j;
				}
			}
		}
	}

	return A[array_index.first][array_index.second];
}

/**
	The function takes an array of hashmaps and merges them together into a single hashmap 
	summing values for same keys. 

	Parameters:	
		array_of_maps – Input array of hashmaps
		output_map - Output hashmap	

	Complexity: O(N) where N is the number of elements in output_map

	Note: when the number of elements in input hashmaps is very large it might make sense to parallelize
	this function too.
*/
unsigned int mergeMaps(HashMapArray2d array_of_maps, HashMap &output_map) {
	
	HashMap::iterator it;
	HashMap::iterator it2;
	output_map = std::move(array_of_maps[0][0]);

	unsigned int map_key = output_map.begin()->first;
	unsigned int max_num_occurances = 1;

	for (index i=0; i != array_of_maps.shape()[0]; ++ i) {
		for (index j=0; j != array_of_maps.shape()[1]; ++j) {
			if (i==0 && j==0) {}
			else {
				for(it = array_of_maps[i][j].begin(); it != array_of_maps[i][j].end(); ++it) {
					it2 = output_map.find(it->first);
					if( it2 == output_map.end()) {
						output_map[it->first] = it->second;
					}
					else {
						it2->second = it2->second + it->second;
						if (max_num_occurances < it2->second) {
							max_num_occurances = it2->second;
							map_key = it2->first;
	
						}
					}
				}
			}
		}
	}

	return map_key;
}

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

vector<size_t> getIndices(size_t input, vector<size_t> sizes) {
	int ndims = (int)sizes.size();
	vector<size_t> result(ndims);
	for (int k = ndims - 1; k >= 0; k--) {
		result[k] = input * 2 % sizes[k];
		input = input * 2 / sizes[k];
	}
	return result;
}

/**
	Prints 2 dimentional array of unsigned integers
*/
void printArray(const UintArray2d &A) {
	cout << "array = " << endl;
	for(index i = 0; i != A.shape()[0]; ++i) {
		for(index j = 0; j != A.shape()[1]; ++j) {
			cout << A[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

/**
	Prints hash map
*/
void printHashMap(HashMap map) {
	HashMap::iterator it;
	cout << "HashMap = " << endl;
	for (it = map.begin(); it != map.end(); ++it) {
		cout << it->first << " " << it->second << endl;
	}
	cout << endl;
}