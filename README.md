# Synopsis and Motivation

This is an assignment that I did for the Princeton Neuroscience Institute. I spent about 6 hours on it. The goal is to block downsample an image using modal values. Efficiency is important and so the algorithm is parallelized using multiple CPU threads. The detailed description of the assignment is given in downsampling.pdf. Please read it first.

# Algorithm Description

The idea of the algorithm is as follows. We are given an original array A of size 2^L1 x 2^L2 x ... 2^Ld. First, we divide A into blocks of size 2x2..x2. Then for each block we create a hash map that counts how many times each element occurs in that block, and also compute the mode of the block. As a result we will have an array of hush maps `A_hash_1` of size 2^(L1-1) x 2^(L2-1) x ... 2^(Ld-1) and 1-downsampled image of the same size.

In the second stage of the algorithms, we divide `A_hash_1`  into blocks of size 2x2..x2. For each block we merge hash maps into a single hash map summing values for same keys, and also compute the mode of the block. As a result we will have an array of hash maps `A_hash_2` of size 2^(L1-2) x 2^(L2-2) x ... 2^(Ld-2) and 2-downsampled image of the same size. We continue merging arrays of hash maps in this manner until we calculate all l=min{L1,...,Ld} downsamples. I think that's the most efficient approach.

As for parallezing the algorithm, I used Thread Building Blocks (TBB) library. TBB is an industry strength library that abstracts access to the multiple processors by allowing the operations to be treated as "tasks", which are allocated to individual cores dynamically by the library's run-time engine, and by automating efficient use of the CPU cache. 

The source files include some additional comments explaining the algorithm and its complexity.
