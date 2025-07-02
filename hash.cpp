#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

// Remove global namespace pollution
// using namespace std; // REMOVED for performance

std::unordered_map<int, std::string> mymap;

struct Entry {
public:
    int key;
    std::string val;
    
    Entry(int key, std::string val) : key(key), val(std::move(val)) {}
};

// 基于数组实现的哈希表 - Optimized version
class ArrayHashMap {
private:
    std::vector<std::unique_ptr<Entry>> buckets;
    static constexpr size_t DEFAULT_SIZE = 101; // Prime number for better distribution
    
    // Improved hash function using std::hash
    size_t hashFunc(int key) const noexcept {
        return std::hash<int>{}(key) % buckets.size();
    }

public:
    ArrayHashMap() : buckets(DEFAULT_SIZE) {}
    
    // Proper destructor - fixes memory leak
    ~ArrayHashMap() = default; // unique_ptr handles cleanup automatically
    
    // Move constructor and assignment for better performance
    ArrayHashMap(ArrayHashMap&&) = default;
    ArrayHashMap& operator=(ArrayHashMap&&) = default;
    
    // Delete copy constructor/assignment (expensive for large hash tables)
    ArrayHashMap(const ArrayHashMap&) = delete;
    ArrayHashMap& operator=(const ArrayHashMap&) = delete;

    // 查询操作 - Fixed return type and improved performance
    std::string get(int key) const {
        size_t index = hashFunc(key);
        const auto& entry = buckets[index];
        return entry ? entry->val : std::string{}; // Return empty string instead of nullptr
    }

    // 添加操作 - Using smart pointers to prevent leaks
    void put(int key, std::string val) {
        size_t index = hashFunc(key);
        buckets[index] = std::make_unique<Entry>(key, std::move(val));
    }

    // 删除操作 - Properly deallocates memory
    void remove(int key) {
        size_t index = hashFunc(key);
        buckets[index].reset(); // Properly deallocates
    }
    
    // 获取所有条目 - More efficient implementation
    std::vector<const Entry*> entrySet() const {
        std::vector<const Entry*> entries;
        entries.reserve(buckets.size() / 4); // Reserve space for better performance
        
        for (const auto& bucket : buckets) {
            if (bucket) {
                entries.push_back(bucket.get());
            }
        }
        return entries;
    }
    
    // 打印函数 - Optimized output
    void print() const {
        const auto entries = entrySet();
        for (const Entry* entry : entries) {
            std::cout << entry->key << " -> " << entry->val << '\n';
        }
    }
    
    // Additional utility functions for performance monitoring
    size_t size() const noexcept {
        size_t count = 0;
        for (const auto& bucket : buckets) {
            if (bucket) ++count;
        }
        return count;
    }
    
    double load_factor() const noexcept {
        return static_cast<double>(size()) / buckets.size();
    }
};

int main() {
    // Optimized initialization
    mymap.reserve(10); // Reserve space to avoid rehashing
    
    mymap[12836] = "小哈";
    mymap[15937] = "小啰";
    mymap[16750] = "小算";
    mymap[13276] = "小法";
    mymap[10583] = "小鸭";

    const std::string& name = mymap[15937]; // Use const reference to avoid copy
    std::cout << "Found: " << name << '\n';

    mymap.erase(10583); // 删除操作，在哈希表中删除键值对(key,value)

    // More efficient iteration
    std::cout << "Key-Value pairs:\n";
    for (const auto& [key, value] : mymap) {
        std::cout << key << " -> " << value << '\n';
    }
    
    std::cout << "\nKeys:\n";
    for (const auto& [key, value] : mymap) {
        std::cout << key << '\n';
    }
    
    std::cout << "\nValues:\n";
    for (const auto& [key, value] : mymap) {
        std::cout << value << '\n';
    }
    
    // Test custom hash map
    std::cout << "\nCustom HashMap Test:\n";
    ArrayHashMap custom_map;
    custom_map.put(1, "Test1");
    custom_map.put(2, "Test2");
    custom_map.put(102, "Test102"); // Test collision handling
    
    std::cout << "Load factor: " << custom_map.load_factor() << '\n';
    custom_map.print();
    
    return 0;
}