// This is an abs function without branching through bitwise operations.

#include <iostream>

int binaryAbs(int originalValue)
{
    int minusOneIfNegative = (originalValue & 1 << 31) / 2147483648;
    int oneIfNegative = minusOneIfNegative * -1;
    int invertedIfNegative = originalValue ^ minusOneIfNegative;
    return invertedIfNegative + oneIfNegative;
}

int main()
{
    int value = -456;
    std::cout << "binaryAbs(" << value << ") = " << binaryAbs(value) << std::endl;
    return 0;
}
