#pragma once
#include <bits/stdc++.h>

#include <utility>
#define ll long long
using namespace std;

struct IMetric {
    virtual ~IMetric() = default;

    virtual void add(any) {
    }

    virtual void set(any) {
    }

    virtual string get_value() =0;

    virtual string get_name() =0;
};

template<typename T>
class Metric : public IMetric {
public:
    Metric(string name): name(name), value(T{}) {
    }

    void add(any value) override {
        T convVal = any_cast<T>(value);
        this->value.fetch_add(convVal);
    }

    void set(any value) override {
        T convVal = any_cast<T>(value);
        this->value.store(convVal);
    }

    string get_value() override {
        T convVal = this->value.exchange(T{});
        return to_string(convVal);
    }

    string get_name() override {
        return name;
    }

private:
    string name;
    atomic<T> value;
};

class Metrics {
public:
    Metrics(string file_name, ll interval = 1000): file_name(file_name), interval(interval) {
        out.open(file_name);
    }

    // void register_metric(IMetric *metric) {
    //     m.lock();
    //     _metrics[metric->get_name()] = metric;
    //     m.unlock();
    // }

    template<typename T>
    void register_metric(string metric_name) {
        m.lock();
        auto metric = new Metric<T>(metric_name);
        _metrics[metric_name] = metric;
        m.unlock();
    }

    //корректно с C++ 20
    void add(string name_metric, auto value) {
        if (_metrics.contains(name_metric)) {
            _metrics[name_metric]->add(any(value));
        } else {
            cerr << '"' << name_metric << '"' << " metric not found" << endl;
        }
    }

    //корректно с C++ 20
    void set(string name_metric, auto value) {
        if (_metrics.contains(name_metric)) {
            _metrics[name_metric]->set(any(value));
        } else {
            cerr << '"' << name_metric << '"' << " metric not found" << endl;
        }
    }

    void start() {
        _flag.store(true);
        _thread = thread([&]() {
            while (_flag) {
                m.lock();
                auto now = chrono::system_clock::now();
                auto t = chrono::system_clock::to_time_t(now);
                auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
                out << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
                        << "." << std::setw(3) << std::setfill('0') << ms.count() << ' ';
                for (auto i: _metrics) {
                    out << '"' << i.first << '"' << ' ' << i.second->get_value() << ' ';
                }
                m.unlock();
                out << '\n';
                out.flush();
                this_thread::sleep_for(chrono::milliseconds(interval));
            }
        });
    }

    void stop() {
        _flag.store(false);
        if (_thread.joinable()) _thread.join();
        out.close();
    }

private:
    map<string, IMetric *> _metrics;
    mutex m;
    ll interval = 1000;
    string file_name;
    thread _thread;
    atomic<bool> _flag = false;
    ofstream out;
};
