#include <iostream>
#include <chrono>
#include <vector>
#include <memory>

using namespace std;

void recursion_function(int depth) {
    int local_var = 0; // Local variable to track stack location approx
    if (depth > 5) return;
    cout << "Recursion depth: " << depth << "\n";
    cout << "Address of local_var at depth: " << &local_var << "\n";
    recursion_function(depth + 1);
}

int main() {

    cout << "--- Stack Memory Trace ---" << endl;
    recursion_function(1);
    cout << "--------------------------" << endl;

    const int size = 10000;
    const int iterations = 10000;

    vector<int> contiguous_array(size);
    vector<unique_ptr<int>> scattered_array;
    for (int i = 0; i < size; i++) scattered_array.push_back(make_unique<int>(i));

    // Measure time for contiguous array access
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        for (auto& val : contiguous_array) val++;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff_contiguous = end - start;

    // Measure time for scattered array access
    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        for (auto& ptr : scattered_array) (*ptr)++;
    }
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff_scattered = end - start;

    cout << "Time taken - contiguous: " << diff_contiguous.count() << "\n";
    cout << "Time taken - scattered: " << diff_scattered.count() << "\n";
    cout << "Performance hit: " << diff_scattered.count() / diff_contiguous.count() << "\n";

    return 0;
}


// shreyas@DESKTOP-E7S0JNG:~/aether/learnings$ ./static_vs_dynamic 
// --- Stack Memory Trace ---
// Recursion depth: 1
// Address of local_var at depth: 0x7ffc048868e4
// Recursion depth: 2
// Address of local_var at depth: 0x7ffc048868b4
// Recursion depth: 3
// Address of local_var at depth: 0x7ffc04886884
// Recursion depth: 4
// Address of local_var at depth: 0x7ffc04886854
// Recursion depth: 5
// Address of local_var at depth: 0x7ffc04886824
// --------------------------
// Time taken - contiguous: 0.895903
// Time taken - scattered: 2.05718
// Performance hit: 2.29621


// shreyas@DESKTOP-E7S0JNG:~/aether/learnings$ ./static_vs_dynamic 
// --- Stack Memory Trace ---
// Recursion depth: 1
// Address of local_var at depth: 0x7ffdbd9b1fe4
// Recursion depth: 2
// Address of local_var at depth: 0x7ffdbd9b1fb4
// Recursion depth: 3
// Address of local_var at depth: 0x7ffdbd9b1f84
// Recursion depth: 4
// Address of local_var at depth: 0x7ffdbd9b1f54
// Recursion depth: 5
// Address of local_var at depth: 0x7ffdbd9b1f24
// --------------------------
// Time taken - contiguous: 0.807075
// Time taken - scattered: 2.07495
// Performance hit: 2.57096
// shreyas@DESKTOP-E7S0JNG:~/aether/learnings$ 