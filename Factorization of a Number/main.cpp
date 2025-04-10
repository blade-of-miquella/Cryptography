#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <numeric>

using namespace std;
vector<unsigned long long> trial_division(unsigned long long n) {
    vector<unsigned long long> factors;
    for (unsigned long long i = 2; i <= sqrt(n); i++) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;}}
    if (n > 1) factors.push_back(n);
    return factors;}

vector<unsigned long long> pollard_p(unsigned long long n) {
    vector<unsigned long long> factors;
    auto f = [&n](unsigned long long x) { return (x * x - x + n) % n; };
    mt19937 gen(random_device{}());
    while (n > 1) {
        if (n % 2 == 0) {
            factors.push_back(2);
            n /= 2;
            continue;}
        uniform_int_distribution<unsigned long long> dist(0, n - 1);
        unsigned long long x = dist(gen), y = x, d = 1, k = 2, i = 0;
        while (d == 1) {
            i++;
            x = f(x);
            d = gcd(abs((long long)(y - x)), n);
            if (d != 1 && d != n) {
                factors.push_back(d);
                n /= d;
                break;}
            if (i == k) {
                y = x;
                k *= 2;}}
        if (d == n) break; }
    if (n > 1) factors.push_back(n); 
    return factors;}

vector<unsigned long long> combined_method(unsigned long long n) {
    vector<unsigned long long> factors = trial_division(n);
    if (!factors.empty() && factors.back() != n) {
        factors = {};
        vector<unsigned long long> pollard_factors = pollard_p(n);
        factors.insert(factors.end(), pollard_factors.begin(), pollard_factors.end());}
    return factors;}

unsigned long long generate_large_number() {
    mt19937 gen(random_device{}());
    uniform_int_distribution<unsigned long long> dist(100000000000000ULL, 999999999999999ULL);
    return dist(gen);}

void compare_methods(unsigned long long n) {
    auto start = chrono::high_resolution_clock::now();
    vector<unsigned long long> trial_factors = trial_division(n);
    auto end = chrono::high_resolution_clock::now();
    cout << "Factors using Trial Division: ";
    for (auto factor : trial_factors) cout << factor << " ";
    cout << endl;
    cout << "Trial Division Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms\n";

    start = chrono::high_resolution_clock::now();
    vector<unsigned long long> pollard_factors = pollard_p(n);
    end = chrono::high_resolution_clock::now();
    cout << "Factors using Pollard's p-Method: ";
    for (auto factor : pollard_factors) cout << factor << " ";
    cout << endl;
    cout << "Pollard's p-Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms\n";

    start = chrono::high_resolution_clock::now();
    vector<unsigned long long> combined_factors = combined_method(n);
    end = chrono::high_resolution_clock::now();
    cout << "Factors using Combined Method: ";
    for (auto factor : combined_factors) cout << factor << " ";
    cout << endl;
    cout << "Combined Method Time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms\n";
}

int main() {
    unsigned long long n = generate_large_number();
    cout << "Generated number (n): " << n << endl;
    compare_methods(n);
    return 0;}
