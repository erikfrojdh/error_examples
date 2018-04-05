
#include <iostream>
#include <stdexcept>

class MyClass {
public:
    MyClass()
    {
        std::cout << "MyClass()\n";
    }
    ~MyClass()
    {
        std::cout << "~MyClass()\n";
    }
    void DoSomething()
    {
        std::cout << "DoSomething() -> Something went wrong\n";
        throw std::runtime_error("Error");
    }
    void AnotherMethod()
    {
        std::cout << "AnotherMethod()\n";
    }
};

int main()
{
    MyClass c;
    try {
        c.DoSomething();
    } catch (std::runtime_error& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }
    //If we catch the error the class can still be used
    //nothing is destroyed
    c.AnotherMethod();

    return 0;
}