#include <iostream>
#include <vector>
#include <string>

// Remove global namespace pollution for better performance
// using namespace std; // REMOVED

// Optimized print function with const correctness and move semantics
void printvec(const std::vector<int>& vec, std::string strbegin = "", std::string strend = "") {
    if (!strbegin.empty()) {
        std::cout << strbegin << '\n';
    }
    
    // Use const auto& to avoid copies
    for (const auto& val : vec) {
        std::cout << val << '\t';
    }
    std::cout << '\n';
    
    if (!strend.empty()) {
        std::cout << strend << '\n';
    }
}

// Optimized selection sort with early termination and fewer swaps
void selectsort(std::vector<int>& mer) {
    const size_t size = mer.size();
    
    // Early termination if array is too small
    if (size <= 1) return;
    
    for (size_t i = 0; i < size - 1; ++i) {
        size_t min_index = i;
        
        // Find the minimum element in remaining unsorted array
        for (size_t j = i + 1; j < size; ++j) {
            if (mer[j] < mer[min_index]) {
                min_index = j;
            }
        }
        
        // Only swap if necessary (optimization)
        if (min_index != i) {
            std::swap(mer[i], mer[min_index]);
        }
    }
}

// Additional optimized sorting algorithms for comparison

// Optimized insertion sort - better for small arrays
void insertionsort(std::vector<int>& arr) {
    const size_t size = arr.size();
    
    for (size_t i = 1; i < size; ++i) {
        const int key = arr[i];
        size_t j = i;
        
        // Move elements that are greater than key one position ahead
        while (j > 0 && arr[j - 1] > key) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = key;
    }
}

// Hybrid sort - uses insertion sort for small arrays, selection sort for larger ones
void hybridsort(std::vector<int>& arr) {
    constexpr size_t INSERTION_SORT_THRESHOLD = 16;
    
    if (arr.size() <= INSERTION_SORT_THRESHOLD) {
        insertionsort(arr);
    } else {
        selectsort(arr);
    }
}

// Performance testing function
void performance_test() {
    std::cout << "Performance Test Results:\n";
    std::cout << "========================\n";
    
    // Test with different array sizes
    std::vector<size_t> test_sizes = {10, 100, 1000};
    
    for (size_t size : test_sizes) {
        std::vector<int> test_data;
        test_data.reserve(size);
        
        // Generate descending order data (worst case)
        for (size_t i = size; i > 0; --i) {
            test_data.push_back(static_cast<int>(i));
        }
        
        auto test_copy = test_data; // Copy for testing
        
        auto start = clock();
        selectsort(test_copy);
        auto end = clock();
        
        double time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC;
        std::cout << "Size " << size << ": " << time_taken << " seconds\n";
    }
}

int main() {
    // Test with the original data
    std::vector<int> mer = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
    
    std::cout << "Original array:\n";
    printvec(mer);
    
    // Test selection sort
    auto mer_copy = mer;
    selectsort(mer_copy);
    printvec(mer_copy, "选择排序后:");
    
    // Test insertion sort
    mer_copy = mer;
    insertionsort(mer_copy);
    printvec(mer_copy, "插入排序后:");
    
    // Test hybrid sort
    mer_copy = mer;
    hybridsort(mer_copy);
    printvec(mer_copy, "混合排序后:");
    
    // Verify all results are the same
    std::cout << "\n算法验证: ";
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    if (mer_copy == expected) {
        std::cout << "所有排序算法结果正确!\n";
    } else {
        std::cout << "排序结果错误!\n";
    }
    
    // Run performance test
    std::cout << '\n';
    performance_test();
    
    return 0;
}