# collapsedKcore
The code is for the CKC algorithm, published in the paper "Finding Critical Users for Social Network Engagement: The Collapsed k-Core Problem", Fan Zhang, Ying Zhang, Lu Qin, Wenjie Zhang, Xuemin Lin, AAAI 2017


# files
collapsedKcore.cpp - source code 

dataset.txt - toy friendship data with 5403 vertices and 20368 edges - data structure: vid \t nid \n... - note that each edge is stored twice and ordered here

the data file is a part of the Gowalla dataset from SNAP: https://snap.stanford.edu/data/


# compile and run
complie with g++ and -O3

run and input the values of 'k' and 'b', such as 5 10 for k=5 and b=10

the program ouputs in result.txt

# note
If you have any question, please contact me by fanzhang.cs@gmail.com.

If you used this code, please kindly cite the paper.

