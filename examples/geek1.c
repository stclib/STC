// https://www.geeksforgeeks.org/maximize-the-number-of-sum-pairs-which-are-divisible-by-k/
// Given an array of N integers and an integer K. The task is to print the maximum number
// of pairs(a[i] + a[j]) possible which are divisible by K.


int a[] = { 1, 2, 2, 3, 2, 4, 10 }; 
//int a[] = { 1, 2, 2, 3, 2, 4, 10, 8, 9, 3, 2, 4 }; 


#ifndef CXX
// C program to implement the above approach 
#include <stdio.h>
#include <stc/cmap.h>

declare_cmap(ii, int, int);
  
// Function to maximize the number of pairs 
int findMaximumPairs(int a[], int n, int k) 
{ 
  
    // Hash-table 
    cmap_ii hash = cmap_init; 
    for (int i = 0; i < n; i++) {
        cmap_ii_insert(&hash, a[i] % k, 0)->value++;
    }
  
    int count = 0; 
  
    // Iterate for all numbers less than hash values 
    c_foreach (it, cmap_ii, hash) { 
  
        // If the number is 0 
        if (it.item->key == 0) { 
            // We take half since same number 
            count += it.item->value / 2; 
            cmap_ii_put(&hash, it.item->key, it.item->key & 1);
        } else { 
            int first = it.item->key;
            int second = k - it.item->key; 

            cmap_ii_entry_t *hf = cmap_ii_find(&hash, first),
                            *hs = cmap_ii_insert(&hash, second, 0);
            // Check for minimal occurrence 
            if (hf->value < hs->value) { 
                // Take the minimal 
                count += hf->value; 
  
                // Subtract the pairs used 
                hs->value -= hf->value; 
                hf->value = 0; 
            } 
            else if (hf->value > hs->value) { 
                // Take the minimal 
                count += hs->value; 
  
                // Subtract the pairs used 
                hf->value -= hs->value; 
                hs->value = 0; 
            } 
            else { 
                // Check if numbers are same 
                if (first == second) { 
  
                    // If same then number of pairs will be half 
                    count += it.item->value / 2; 
  
                    // Check for remaining 
                    hf->value = (it.item->key & 1); 
                } 
                else { 
                    // Store the number of pairs 
                    count += hf->value; 
                    hf->value = 0; 
                    hs->value = 0; 
                } 
            } 
        } 
    } 
  
    return count; 
} 
  
// Driver code 
int main() 
{ 
    int n = sizeof(a) / sizeof(a[0]); 
    int k = 2; 
    printf("C : %d\n", findMaximumPairs(a, n, k));
  
    return 0;
} 


#else


// C++ program to implement the above 
// approach 
#include <bits/stdc++.h> 
using namespace std; 
  
// Function to maximize the number of pairs 
int findMaximumPairs(int a[], int n, int k) 
{ 
  
    // Hash-table 
    unordered_map<int, int> hash; 
    for (int i = 0; i < n; i++) 
        hash[a[i] % k]++; 
  
    int count = 0; 
  
    // Iterate for all numbers less than hash values 
    for (auto it : hash) { 
  
        // If the number is 0 
        if (it.first == 0) { 
  
            // We take half since same number 
            count += it.second / 2; 
            if (it.first % 2 == 0) 
                hash[it.first] = 0; 
            else
                hash[it.first] = 1; 
        } 
        else { 
  
            int first = it.first; 
            int second = k - it.first; 
  
            // Check for minimal occurrence 
            if (hash[first] < hash[second]) { 
                // Take the minimal 
                count += hash[first]; 
  
                // Subtract the pairs used 
                hash[second] -= hash[first]; 
                hash[first] = 0; 
            } 
            else if (hash[first] > hash[second]) { 
                // Take the minimal 
                count += hash[second]; 
  
                // Subtract the pairs used 
                hash[first] -= hash[second]; 
                hash[second] = 0; 
            } 
            else { 
                // Check if numbers are same 
                if (first == second) { 
  
                    // If same then number of pairs will be half 
                    count += it.second / 2; 
  
                    // Check for remaining 
                    if (it.first % 2 == 0) 
                        hash[it.first] = 0; 
                    else
                        hash[it.first] = 1; 
                } 
                else { 
  
                    // Store the number of pairs 
                    count += hash[first]; 
                    hash[first] = 0; 
                    hash[second] = 0; 
                } 
            } 
        } 
    } 
  
    return count; 
} 
  
// Driver code 
int main() 
{ 
    int n = sizeof(a) / sizeof(a[0]); 
    int k = 2; 
    cout << "C++ : " << findMaximumPairs(a, n, k); 
  
    return 0; 
} 

#endif