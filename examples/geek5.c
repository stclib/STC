/*
https://www.geeksforgeeks.org/number-of-times-the-given-string-occurs-in-the-array-in-the-range-l-r/

Number of times the given string occurs in the array in the range [l, r]
Given an array of strings arr[] and two integers l and r, the task is to find the number of times 
the given string str occurs in the array in the range [l, r] (1-based indexing). Note that the
strings contain only lowercase letters.

Examples:

Input: arr[] = {"abc", "def", "abc"}, L = 1, R = 2, str = "abc"
Output: 1

Input: arr[] = {"abc", "def", "abc"}, L = 1, R = 3, str = "ghf"
Output: 0
*/

#ifndef CXX

#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cstr.h>

declare_cvec(i, int);
declare_cmap_strkey(sv, cvec_i, cvec_i_destroy);

  
// Function to return the number of occurrences of 
int NumOccurrences(const char* arr[], int n, const char* str, int L, int R) 
{ 
    // To store the indices of strings in the array 
    cmap_sv M = cmap_init; 
    for (int i = 0; i < n; i++) { 
        const char* temp = arr[i]; 
        cmap_sv_entry_t *it = cmap_sv_find(&M, temp); 
  
        // If current string doesn't 
        // have an entry in the map 
        // then create the entry 
        if (it == NULL) { 
            cvec_i A = cvec_init; 
            cvec_i_push_back(&A, i + 1);
            cmap_sv_put(&M, temp, A); 
        } 
        else {
            cvec_i_push_back(&it->value, i + 1);
        } 
    } 
  
    cmap_sv_entry_t *it = cmap_sv_find(&M, str);

    // If the given string is not 
    // present in the array 
    if (it == NULL)
        return 0; 
  
    // If the given string is present 
    // in the array 
    cvec_i A = it->value;
    int y = 0, x = 0; 
    for (; y < cvec_size(A); ++y) if (A.data[y] > R) break;
    for (; x < cvec_size(A); ++x) if (A.data[x] > L - 1) break;

    cmap_sv_destroy(&M);
    return (y - x); 
} 
  
// Driver code 
int main() 
{ 
    const char* arr[] = { "abc", "abcabc", "abc" }; 
    int n = sizeof(arr) / sizeof(*arr); 
    int L = 1; 
    int R = 3; 
    const char* str = "abc"; 
  
    printf("%d\n", NumOccurrences(arr, n, str, L, R)); 
  
    return 0; 
} 

#else // =============================== c++ ==================================

// C++ implementation of the approach 
#include <bits/stdc++.h> 
using namespace std; 
  
// Function to return the number of occurrences of 
int NumOccurrences(string arr[], int n, string str, int L, int R) 
{ 
    // To store the indices of strings in the array 
    unordered_map<string, vector<int> > M; 
    for (int i = 0; i < n; i++) { 
        string temp = arr[i]; 
        auto it = M.find(temp); 
  
        // If current string doesn't 
        // have an entry in the map 
        // then create the entry 
        if (it == M.end()) { 
            vector<int> A; 
            A.push_back(i + 1); 
            M.insert(make_pair(temp, A)); 
        } 
        else { 
            it->second.push_back(i + 1); 
        } 
    } 
  
    auto it = M.find(str); 
  
    // If the given string is not 
    // present in the array 
    if (it == M.end()) 
        return 0; 
  
    // If the given string is present 
    // in the array 
    vector<int> A = it->second; 
    int y = upper_bound(A.begin(), A.end(), R) - A.begin(); 
    int x = upper_bound(A.begin(), A.end(), L - 1) - A.begin(); 
  
    return (y - x); 
} 
  
// Driver code 
int main() 
{ 
    string arr[] = { "abc", "abcabc", "abc" }; 
    int n = sizeof(arr) / sizeof(string); 
    int L = 1; 
    int R = 3; 
    string str = "abc"; 
  
    cout << NumOccurrences(arr, n, str, L, R); 
  
    return 0; 
}

#endif