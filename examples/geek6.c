// https://www.geeksforgeeks.org/find-the-smallest-positive-number-missing-from-an-unsorted-array-hashing-implementation/
/*
Find the smallest positive number missing from an unsorted array : Hashing Implementation
Given an unsorted array with both positive and negative elements including 0. The task is
to find the smallest positive number missing from the array in O(N) time.

Examples:

Input: arr[] = {-5, 2, 0, -1, -10, 15}
Output: 1

Input: arr[] = {0, 1, 2, 3, 4, 5}
Output: 6

Input: arr[] = {1, 1, 1, 0, -1, -2}
Output: 2

We can use hashing to solve this problem. The idea is to build a hash table of all positive
elements in the given array. Once the hash table is built. We can look in the hash table for
all positive integers, starting from 1. As soon as we find a number which is not there in the
hash table, we return it.
We can use the unordered_map in C++ to implement the hash which allow to perform look up 
operation in almost O(1) time complexity.
*/

#ifndef CXX

// C program to find the smallest 
// positive missing number 

#include <stdio.h>  
#include <stc/cmap.h>
c_cset(i, int);

// Function to find the smallest positive 
// missing number 
int missingNumber(int a[], int n) 
{ 
    // Declaring an unordered_map 
    cset_i mp = cset_ini; 
  
    // if array value is positive 
    // store it in map 
    for (int i = 0; i < n; i++) { 
        if (a[i] > 0) 
            cset_i_insert(&mp, a[i]);
    } 
  
    // index value set to 1 
    int index = 1; 
  
    // Return the first value starting 
    // from 1 which does not exists in map 
    while (1) { 
        if (cset_i_find(&mp, index) == NULL) {
            return index; 
        } 
  
        index++; 
    }
    cset_i_destroy(&mp);
} 
  
// Driver code 
int main() 
{ 
    int a[] = { 1, 1, 1, 0, -1, -2 }; 
    int size = sizeof(a) / sizeof(a[0]); 
  
    printf("Smallest positive missing number is : %d\n",
           missingNumber(a, size)); 
  
    return 0; 
} 

#else

// C++ program to find the smallest 
// positive missing number 
  
#include <bits/stdc++.h> 
using namespace std; 
  
// Function to find the smallest positive 
// missing number 
int missingNumber(int a[], int n) 
{ 
    // Declaring an unordered_map 
    unordered_map<int, int> mp; 
  
    // if array value is positive 
    // store it in map 
    for (int i = 0; i < n; i++) { 
        if (a[i] > 0) 
            mp[a[i]]++; 
    } 
  
    // index value set to 1 
    int index = 1; 
  
    // Return the first value starting 
    // from 1 which does not exists in map 
    while (1) { 
        if (mp.find(index) == mp.end()) { 
            return index; 
        } 
  
        index++; 
    } 
} 
  
// Driver code 
int main() 
{ 
    int a[] = { 1, 1, 1, 0, -1, -2 }; 
    int size = sizeof(a) / sizeof(a[0]); 
  
    cout << "Smallest positive missing number is : "
         << missingNumber(a, size) << endl; 
  
    return 0; 
} 

#endif