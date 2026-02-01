#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <unistd.h>

using namespace std;

// Global variables (Data Segment)
int global_initialized = 42; 
int global_uninitialized; // BSS Segment

void print_proc_maps() {
    ifstream maps("/proc/self/maps");
    cout << "\n--- [ /proc/self/maps ] ---\n";
    cout << maps.rdbuf();
    cout << "---------------------------\n";
}

int main() {
    // Stack variable
    int stack_var = 100;

    // Heap variable (Free Store)
    auto heap_var = make_unique<int>(200);

    // Static variable (Data Segment)
    static int static_var = 5;

    cout << "--- Segment Addresses ---" << endl;
    cout << "Code (.text) approx:  " << (void*)main << endl;
    cout << "Global (Data):        " << &global_initialized << endl;
    cout << "Global (BSS):         " << &global_uninitialized << endl;
    cout << "Static (Data):        " << &static_var << endl;
    cout << "Heap (Free Store):    " << heap_var.get() << endl;
    cout << "Stack (Automatic):    " << &stack_var << endl;

    print_proc_maps();

    return 0;
}