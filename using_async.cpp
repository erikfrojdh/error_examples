#include <chrono>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <random>

template <typename T>
T element_if_all_equal(std::vector<T> v)
{
    //if the vector is emty we cannot return an element
    //this probably should be handled by the caller
    if (v.size() == 0)
        throw(std::runtime_error("comparing on emty vector"));

    //if all elements are equal we return the first element
    //TODO how do we treat the case where elemets are not equal?
    if (std::equal(v.begin() + 1, v.end(), v.begin()))
    {
        return v[0];
    }
    else
    {
        //runtime error waiting to happen!
        return -1;
    }
}

class slsDetector
{
  public:
    slsDetector(){};

    double exposureTime(double t)
    {
        // Code to sleep a random number of seconds, simulating work or network delay
        // just to show that functions are run in parallel
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, 5);
        auto sleep_time = uni(rng);
        std::cout << det_id_ << ":slsDetector::exposureTime(" << t << ") --> sleep for " << sleep_time << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
        std::cout << det_id_ << ":done\n";

        //set exposure time
        exposureTime_ = t;
        return exposureTime();
    }

    double exposureTime()
    {
        return exposureTime_;
    }

    double someOtherParameter(int i, double v)
    {
        std::cout << det_id_ << ":slsDetector::someOtherParameter(" << i << ", " << v << ")\n";
        if (det_id_ == 2)
            throw(std::runtime_error("Detector " + std::to_string(det_id_) + " has a problem"));
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

class multiSlsDetector
{
  public:
    multiSlsDetector(size_t N)
        : n_modules_(N)
    {
        //initialize detectors, this should probably be done using vectors or unique pointer
        //but using normal pointers for similarity with the current implementation
        slsDetector_ = new slsDetector[N];
        for (size_t i = 0; i < N; ++i)
            (*this)[i]->id(i);
    }
    ~multiSlsDetector()
    {
        delete[] slsDetector_;
    }
    slsDetector *operator[](size_t i)
    {
        //Providing access to detectors with range checking
        if (i < n_modules_)
            return &slsDetector_[i];
        else
            throw(std::range_error("Detector does not exist"));
    }
    size_t n_modules_ules() { return n_modules_; }

    double exposureTime(double t)
    {
        // using static cast to resolve overloaded functions
        // using tt = double(slsDetector::*)(double);
        auto v = parallel_call(static_cast<double (slsDetector::*)(double)>(&slsDetector::exposureTime), t);
        return element_if_all_equal(v);
    }

    double exposureTime()
    {
        auto v = parallel_call(static_cast<double (slsDetector::*)()>(&slsDetector::exposureTime));
        return v[0];
    }

    //overload from multi to set exposure time of one detector
    double exposureTime(double t, size_t det_id)
    {
        return (*this)[det_id]->exposureTime(t);
    }

    void someOtherParameter(int i, double d){
        auto v = parallel_call(&slsDetector::someOtherParameter, i, d);
    }

    // Template for calling a member function for each slsDetector
    // uses async to do this in parallel with 
    template <typename... CT, typename RT>
    std::vector<RT> parallel_call(RT (slsDetector::*somefunc)(CT...), CT... Args)
    {
        std::vector<std::future<RT>> futures;
        for (size_t det_id = 0; det_id < n_modules_; ++det_id)
        {
            futures.push_back(std::async(somefunc, (*this)[det_id], Args...));
        }
        std::vector<RT> result;
        for (auto &i : futures)
            result.push_back(i.get());

        return result;
    }

    // Overload for functions that does not have a return type
    // vector of void would not be so good...
    template <typename... CT>
    void parallel_call(void (slsDetector::*somefunc)(CT...), CT... Args)
    {
        std::vector<std::future<void>> futures;
        for (size_t det_id = 0; det_id < n_modules_; ++det_id)
        {
            futures.push_back(std::async(somefunc, (*this)[det_id], Args...));
        }
        for (auto &i : futures)
            i.get();
    }

  private:
    slsDetector *slsDetector_;
    size_t n_modules_;
};

int main()
{
    multiSlsDetector d{5};

    //Set the exposure time of all detectors
    d.exposureTime(7.9);
    auto t = d.exposureTime();
    std::cout << "Got: " << t << "\n";

    //Set the exposure time of a single detector
    d.exposureTime(5.36, 2);

    //call an function where one module throws an exception
    //the exception is propagated from the thread that async launched
    //and caught at the top level. We can implement a class of exception
    //that carries information about wich detector etc.
    try{
        d.someOtherParameter(5, 4.4);
    }
    catch(std::runtime_error& e){
        std::cout << "Caught: " << e.what() << "\n";
    }
    

    return 0;
}