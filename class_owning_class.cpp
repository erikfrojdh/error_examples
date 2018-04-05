
#include <iostream>
#include <stdexcept>
#include <vector>

class AnotherClass {
public:
    AnotherClass(int i)
    {
        i_ = i;
        std::cout << "AnotherClass(" << i << ")\n";
    }
    ~AnotherClass()
    {
        std::cout << "~AnotherClass(" << i_ << ")\n";
    }
    void DoSomething()
    {
        std::cout << "AnotherClass(" << i_ << ") DoSomething()\n";
        if (i_ == 5)
            throw std::runtime_error("Hey!");
    }
    void Print()
    {
        std::cout << "I'm " << i_ << "\n";
    }

private:
    int i_;
};

class MyClass {
public:
    MyClass()
    {
        std::cout << "MyClass()\n";
        for (int i = 0; i < 10; ++i) {
            v.push_back(new AnotherClass(i));
        }
    }
    ~MyClass()
    {
        std::cout << "~MyClass()\n";
        for (size_t i = 0; i < v.size(); ++i)
            delete v[i];
    }
    void IterateAnotherClass(){
        for (size_t i = 0; i<v.size(); ++i){
            v[i]->DoSomething();
        }
    }
    AnotherClass* getAnotherClass(int i){
        return v[i];
    }

private:
    //using a vector of pointers can also use something that
    //owns the objects
    std::vector<AnotherClass*> v;
};

int main()
{
    MyClass c;
    try{
        c.IterateAnotherClass();
    }
    catch(std::runtime_error& e){
        std::cout << "Caught: " << e.what() << "\n";
    }

    //Everything is till in place and usable
    AnotherClass* p = c.getAnotherClass(3);
    p->DoSomething();
    return 0;
}