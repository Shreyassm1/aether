#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;
using namespace chrono;

int main() {
    const int N = 10000000;
    vector<int> data(N);
    for (int i = 0; i < N; i++) data[i] = i;

    // Sequential indices
    vector<int> seq(N);
    for (int i = 0; i < N; i++) seq[i] = i;

    // Random indices
    vector<int> rnd = seq;
    shuffle(rnd.begin(), rnd.end(), mt19937{random_device{}()});

    cout << "--- CPU Cache Locality ---\n";

    // Sequential access (fast - cache friendly)
    auto t1 = high_resolution_clock::now();
    long long sum1 = 0;
    for (int idx : seq) sum1 += data[idx];
    auto t2 = high_resolution_clock::now();
    cout << "Sequential: " << duration<double>(t2 - t1).count() << "s | Sum: " << sum1 << endl;

    // Random access (slow - cache misses)
    t1 = high_resolution_clock::now();
    long long sum2 = 0;
    for (int idx : rnd) sum2 += data[idx];
    t2 = high_resolution_clock::now();
    cout << "Random:     " << duration<double>(t2 - t1).count() << "s | Sum: " << sum2 << endl;

    return 0;
}


// shreyas@DESKTOP-E7S0JNG:~/aether$ ./cache_behaviour 
// --- CPU Cache Locality ---
// Sequential: 0.10393s | Sum: 49999995000000
// Random:     0.963962s | Sum: 49999995000000