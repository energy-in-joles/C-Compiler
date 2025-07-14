#include <stdio.h>
#include <stdint.h>  // For uint64_t

void print_double_bits(double value) {
    // Reinterpret the bits of the double as a 64-bit unsigned integer
    union {
        double d;
        uint64_t i;
    } converter;

    converter.d = value;

    // Print the 64-bit representation in hexadecimal
    printf("Double value: %f\n", value);
    printf("Exact bits (hex): 0x%016lx\n", converter.i);
}

double foo(double x);

int main()
{
    double result = foo(1.0);
    print_double_bits(result);
    // printf("Example function returned: %f\n", foo(1.0));
    return !(foo(1.0) == 1.0);
}
