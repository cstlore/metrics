#include <bits/stdc++.h>
#define ll long long
#define ld long double
#include "metrics.hpp"
using namespace std;
random_device rd;
mt19937 rnd(rd());

int main() {
    Metrics metrics("metrics.txt");
    metrics.register_metric<double>("CPU");
    metrics.register_metric<int>("HTTP requests RPS");
    metrics.add("HTTP requests RPS", 5);
    metrics.add("HTTP requests RPS", 5);
    metrics.add("HTTP requests RPS", 5);
    // две механики: metrics.add и metrics.set
    metrics.start();
    this_thread::sleep_for(chrono::milliseconds(1000));
    uniform_real_distribution<double> cpu_dist(0, 8);
    uniform_int_distribution<int> http_dist(0, INT_MAX);
    for (ll i = 0; i < 30; ++i) {
        metrics.set("HTTP requests RPS", http_dist(rnd));
        metrics.set("CPU", cpu_dist(rnd));
        this_thread::sleep_for(chrono::milliseconds(300));
    }
    metrics.stop();
    return 0;
}
