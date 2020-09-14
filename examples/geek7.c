/*
https://www.geeksforgeeks.org/find-the-k-smallest-numbers-after-deleting-given-elements/

Find the k smallest numbers after deleting given elements
Given an array of integers, find the k smallest numbers after deleting given elements. In case of repeating elements delete only one instance in the given array for every instance of element present in the array containing the elements to be deleted.
Assume that there are at least k elements left in the array after making n deletions.
Examples:

Input : array[] = { 5, 12, 33, 4, 56, 12, 20 }, del[] = { 12, 56, 5 }, k = 3
Output : 4 12 20
Explanation : After deletions { 33, 4, 12, 20 } will be left. Print top 3 smallest elements from it.

Approach :
Insert all the numbers in the hash map which are to be deleted from the array, so that we can check if the element in the array is also present in the Delete-array in O(1) time.
Traverse through the array. Check if the element is present in the hash map.
If present, erase it from the hash map. Else, insert it into a Min heap.
After inserting all the elements excluding the ones which are to be deleted, Pop out k elements from the Min heap.
*/

#ifndef CXX

// C implementation of the approach 

#include <stdio.h>
#include <stc/clist.h>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cpqueue.h>

declare_cmap(ii, int, int);
declare_cvec(i, int);
declare_cpqueue(i, cvec_i, >);

// Find k minimum element from arr[0..m-1] after deleting
// elements from del[0..n-1]
void findElementsAfterDel(int arr[], int m, int del[], 
                          int n, int k) 
{ 
    // Hash Map of the numbers to be deleted 
    cmap_ii mp = cmap_ini; 
    for (int i = 0; i < n; ++i) { 
  
        // Increment the count of del[i] 
        cmap_ii_emplace(&mp, del[i], 0).first->value++; 
    } 
  
    cpqueue_i heap = cpqueue_i_init(); 
  
    for (int i = 0; i < m; ++i) { 
  
        // Search if the element is present 
        cmap_ii_entry_t *e = cmap_ii_find(&mp, arr[i]);
        if (e != NULL) { 
  
            // Decrement its frequency 
            e->value--; 
  
            // If the frequency becomes 0, 
            // erase it from the map 
            if (e->value == 0) 
                cmap_ii_erase_entry(&mp, e);
        } 
  
        // Else push it in the min heap 
        else
            cpqueue_i_push(&heap, arr[i]); 
    } 
  
    // Print top k elements in the min heap 
    for (int i = 0; i < k; ++i) { 
        printf("%d ", *cpqueue_i_top(&heap)); 
  
        // Pop the top element 
        cpqueue_i_pop(&heap); 
    }
    cpqueue_i_destroy(&heap);
} 
  
int main() 
{ 
    int array[] = { 5, 12, 33, 4, 56, 12, 20 }; 
    int m = sizeof(array) / sizeof(array[0]); 
  
    int del[] = { 12, 56, 5 }; 
    int n = sizeof(del) / sizeof(del[0]); 
  
    int k = 3; 
  
    findElementsAfterDel(array, m, del, n, k); 
    return 0; 
} 

#else // =====================================================
// C++ implementation of the approach 

#include <iostream> 
#include <queue> 
#include <unordered_map> 
#include <vector> 
using namespace std; 
  
// Find k minimum element from arr[0..m-1] after deleting 
// elements from del[0..n-1] 
void findElementsAfterDel(int arr[], int m, int del[], 
                          int n, int k) 
{ 
    // Hash Map of the numbers to be deleted 
    unordered_map<int, int> mp; 
    for (int i = 0; i < n; ++i) { 
  
        // Increment the count of del[i] 
        mp[del[i]]++; 
    } 
  
    priority_queue<int, vector<int>, greater<int> > heap; 
  
    for (int i = 0; i < m; ++i) { 
  
        // Search if the element is present 
        if (mp.find(arr[i]) != mp.end()) { 
  
            // Decrement its frequency 
            mp[arr[i]]--; 
  
            // If the frequency becomes 0, 
            // erase it from the map 
            if (mp[arr[i]] == 0) 
                mp.erase(arr[i]); 
        } 
  
        // Else push it in the min heap 
        else
            heap.push(arr[i]); 
    } 
  
    // Print top k elements in the min heap 
    for (int i = 0; i < k; ++i) { 
        cout << heap.top() << " "; 
  
        // Pop the top element 
        heap.pop(); 
    } 
} 
  
int main() 
{ 
    int array[] = { 5, 12, 33, 4, 56, 12, 20 }; 
    int m = sizeof(array) / sizeof(array[0]); 
  
    int del[] = { 12, 56, 5 }; 
    int n = sizeof(del) / sizeof(del[0]); 
  
    int k = 3; 
  
    findElementsAfterDel(array, m, del, n, k); 
    return 0; 
} 

#endif