#include <iostream>
#include <stdexcept>
#include <vector>

class AnotherClass {
public:
    AnotherClass()
    {
        std::cout << "AnotherClass()\n";
    }
    ~AnotherClass()
    {
        //this is called however
        std::cout << "~AnotherClass()\n";
    }
};

class MyClass {
public:
    MyClass()
        : a()
    {
        std::cout << "MyClass()\n";
        some_memory = new int[50]; //this memory will be leaked
        v           = std::vector<int>(10); //this gets cleaned up when the vector is destroyed
        throw std::runtime_error("Error");
    }
    ~MyClass()
    {
        //Destructor is not run if an exception
        //is thrown in the constructor
        std::cout << "~MyClass()\n";
        delete[] some_memory;
    }

    void func()
    {
    }

private:
    int* some_memory;
    std::vector<int> v;
    AnotherClass a;
};

int main()
{
    try {
        MyClass a;
        a.func();
    } catch (std::runtime_error& e) {
        std::cout << "got a runtime error: " << e.what() << "\n";
    }

    std::cout << "end\n";

    return 0;
}