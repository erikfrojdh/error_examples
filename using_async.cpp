#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <random>

class slsDetector {
public:
    slsDetector(){};
    double exposureTime(double t)
    {
        // Code to sleep a random number of seconds, simulating work or network delay
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, 5);
        auto sleep_time = uni(rng);
        std::cout << det_id_ << ":slsDetector::exposureTime(" << t << ") --> sleep for " << sleep_time << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
        std::cout << det_id_ << ":done\n";

        //set exposure time
        exposureTime_ = t;
        return exposureTime_;
    }

    // double exposureTime()
    // {
    //     return exposureTime_;
    // }

    double someOtherParameter(int i, double v)
    {
        std::cout << det_id_ << ":slsDetector::someOtherParameter(" << i << ", " << v << ")\n";
        return static_cast<double>(i);
    }
    void do_something()
    {
        std::cout << det_id_ << ":slsDetector::do_something()\n";
    }

    size_t id()
    {
        return det_id_;
    }
    void id(size_t det_id)
    {
        det_id_ = det_id;
    }

private:
    double exposureTime_;
    size_t det_id_;
};

class multiSlsDetector {
public:
    multiSlsDetector(size_t N)
        : n_mod(N)
    {
        slsDetector_ = new slsDetector[N];
        for (size_t i = 0; i < N; ++i)
            (*this)[i]->id(i);
    }
    ~multiSlsDetector()
    {
        delete[] slsDetector_;
    }
    slsDetector* operator[](size_t i)
    {
        return &slsDetector_[i];
    }
    size_t n_modules() { return n_mod; }


    double exposureTime(double t)
    {
        auto v = multi_task(&slsDetector::exposureTime, t);
        return v[0];
    }

    double exposureTime(double t, size_t det_id)
    {
        return (*this)[det_id]->exposureTime(t);
    }


    // Template for calling a member function for each slsDetector
    template <typename... CT, typename RT>
    std::vector<RT> multi_task(RT (slsDetector::*somefunc)(CT...), CT... Args)
    {
        std::vector<std::future<RT>> futures;
        for (size_t det_id = 0; det_id < n_mod; ++det_id) {
            futures.push_back(std::async(somefunc, (*this)[det_id], Args...));
        }
        std::vector<RT> result;
        for (auto& i : futures)
            result.push_back(i.get());

        return result;
    }

    // Overload for functions that does not have a return type
    template <typename... CT>
    void multi_task(void (slsDetector::*somefunc)(CT...), CT... Args)
    {
        std::vector<std::future<void>> futures;
        for (size_t det_id = 0; det_id < n_mod; ++det_id) {
            futures.push_back(std::async(somefunc, (*this)[det_id], Args...));
        }
        for (auto& i : futures)
            i.get();
    }


private:
    slsDetector* slsDetector_;
    size_t n_mod;
};

int main()
{
    multiSlsDetector d{ 5 };

    //Set the exposure time of all detectors
    d.exposureTime(3.21);

    //Set the exposure time of a single detector
    d.exposureTime(5.36, 2);

    return 0;
}