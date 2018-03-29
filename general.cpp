
#include <iostream>
#include <exception>

int division1(double numerator, double denominator, double &result)
{
    if (denominator == 0)
    {
        return 1;
    }

    else
    {
        result = numerator / denominator;
        return 0;
    }
}

double division2(double numerator, double denominator, int &error_code)
{
    if (denominator == 0)
    {
        error_code = 1;
        return 0;
    }
    else
    {
        return numerator / denominator;
    }
}

double division3(double numerator, double denominator)
{
    if (denominator == 0)
        throw std::runtime_error("Division by zero!");
    return numerator / denominator;
}

int main()
{
    //C style error code
    double result1 = 0;
    if (division1(23, 0, result1))
        std::cout << "Division by zero in division1\n";
    else
        std::cout << "Result1: " << result1 << "\n";

    //Error code by reference
    int error_code = 0;
    double result2 = division2(4, 0, error_code);
    if (error_code)
        std::cout << "Division by zero in division2\n";

    //Exception
    double result3 = 0;
    try
    {
        double result3 = division3(5, 0);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Caught exception in division3: \"" << e.what() << "\"\n";
    }

    return 0;
}