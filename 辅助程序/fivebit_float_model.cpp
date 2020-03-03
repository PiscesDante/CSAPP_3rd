#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

const int BIAS = 1;
int get_expo_value(std::string& expo_bits);
double get_frac_value(const std::string& frac_bits);
void float_model(const std::string& float_bits);

int main(int argc, char** argv)
{
    std::fstream file_stream("float_numbers.txt");
    if (file_stream.is_open()) {
        for (std::string line; getline(file_stream, line); ) float_model(line);
        file_stream.close();
    }
    return 0;
}

int get_expo_value(std::string& expo_bits)
{
    int ret = 0;
    int base = 1;
    std::reverse(expo_bits.begin(), expo_bits.end());
    for (auto bit : expo_bits) {
        if (bit == '1') ret += base;
        base *= 2;
    }
    return ret;
}

double get_frac_value(const std::string& frac_bits)
{
    double ret = 0;
    double base = 0.5;
    for (auto bit : frac_bits) {
        if (bit == '1') ret += base;
        base /= 2;
    }
    return ret;
}

void float_model(const std::string& float_bits)
{
    char sign_bit = float_bits[0];
    std::string expo_bits = float_bits.substr(1, 2);
    std::string frac_bits = float_bits.substr(3, 2);

    int expo_value;
    int biased_expo_value;
    int weight_of_expo;
    double frac_value;
    double significand_value;
    double frac_value_of_float;
    double decimal_value;

    if (expo_bits == "11") {        /* status: 3 */
        std::cout << float_bits << " --status 3-- ";
        if (frac_bits == "00") {
            if (sign_bit == '0') std::cout << "Positive Infinity" << std::endl;
            else std::cout << "Negative Infinity" << std::endl;
        } else {
            std::cout << "Not a Number" << std::endl;
        }
    } else if (expo_bits == "00") { /* status: 2 */
        std::cout << float_bits << " --status 2-- ";
        expo_value = 0;
        std::cout << expo_value << " | ";
        biased_expo_value = 1 - BIAS;
        std::cout << biased_expo_value << " | ";
        weight_of_expo = pow(2, biased_expo_value);
        std::cout << weight_of_expo << " | ";
        frac_value = get_frac_value(frac_bits);
        std::cout << frac_value << " | ";
        significand_value = frac_value;
        std::cout << significand_value << " | ";
        frac_value_of_float = weight_of_expo * significand_value;
        std::cout << frac_value_of_float << " | ";
        decimal_value = (sign_bit == '0' ? 1 : -1) * frac_value_of_float;
        std::cout << decimal_value << std::endl;
    } else {                         /* status: 1 */
        std::cout << float_bits << " --status 1-- ";
        expo_value = get_expo_value(expo_bits);
        std::cout << expo_value << " | ";
        biased_expo_value = expo_value - BIAS;
        std::cout << biased_expo_value << " | "; // E
        weight_of_expo = pow(2, biased_expo_value);
        std::cout << weight_of_expo << " | ";
        frac_value = get_frac_value(frac_bits);
        std::cout << frac_value << " | ";
        significand_value = frac_value + 1;
        std::cout << significand_value << " | ";
        frac_value_of_float = weight_of_expo * significand_value;
        std::cout << frac_value_of_float << " | ";
        decimal_value = (sign_bit == '0' ? 1 : -1) * frac_value_of_float;
        std::cout << decimal_value << std::endl;
    }
}

// Result:
// 00000 --status 2-- 0 | 0 | 1 | 0 | 0 | 0 | 0
// 00001 --status 2-- 0 | 0 | 1 | 0.25 | 0.25 | 0.25 | 0.25
// 00010 --status 2-- 0 | 0 | 1 | 0.5 | 0.5 | 0.5 | 0.5
// 00011 --status 2-- 0 | 0 | 1 | 0.75 | 0.75 | 0.75 | 0.75
// 00100 --status 1-- 1 | 0 | 1 | 0 | 1 | 1 | 1
// 00101 --status 1-- 1 | 0 | 1 | 0.25 | 1.25 | 1.25 | 1.25
// 00110 --status 1-- 1 | 0 | 1 | 0.5 | 1.5 | 1.5 | 1.5
// 00111 --status 1-- 1 | 0 | 1 | 0.75 | 1.75 | 1.75 | 1.75
// 01000 --status 1-- 2 | 1 | 2 | 0 | 1 | 2 | 2
// 01001 --status 1-- 2 | 1 | 2 | 0.25 | 1.25 | 2.5 | 2.5
// 01010 --status 1-- 2 | 1 | 2 | 0.5 | 1.5 | 3 | 3
// 01011 --status 1-- 2 | 1 | 2 | 0.75 | 1.75 | 3.5 | 3.5
// 01100 --status 3-- Positive Infinity
// 01101 --status 3-- Not a Number
// 01110 --status 3-- Not a Number
// 01111 --status 3-- Not a Number