/**
 * float 标准：1 + 8 + 23
 * 11000000111001 - 12345
 * 1.1000000111001 * 2^13
 * status - 1
 * frac = 1000000111001
 * expo = 13 + 2^{8 - 1} - 1 = 140 = 10001100
 * sign = 0
 * 0 10001100 10000001110010000000000
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <stack>
#include <map>

std::map<std::string, char> dict_bin_to_hex = {
    std::make_pair("0000", '0'),
    std::make_pair("0001", '1'),
    std::make_pair("0010", '2'),
    std::make_pair("0011", '3'),
    std::make_pair("0100", '4'),
    std::make_pair("0101", '5'),
    std::make_pair("0110", '6'),
    std::make_pair("0111", '7'),
    std::make_pair("1000", '8'),
    std::make_pair("1001", '9'),
    std::make_pair("1010", 'A'),
    std::make_pair("1011", 'B'),
    std::make_pair("1100", 'C'),
    std::make_pair("1101", 'D'),
    std::make_pair("1110", 'E'),
    std::make_pair("1111", 'F'),
};

void show_bitpatterns(int decimal_value);
std::string decimal_to_binary(const std::string& deci_string);
std::string binary_to_hexadecimal(const std::string& bina_string);

int main()
{
    int val;
    std::cout << "Enter unsigned number: ";
    std::cin >> val;
    show_bitpatterns(val);
    return 0;
}

std::string decimal_to_binary(const std::string& deci_string)
{
    int deci_number = std::stoi(deci_string);
    std::stack<int> bit_stack;
    while (deci_number / 2 != 1) {
        bit_stack.push(deci_number % 2);
        deci_number /= 2;
    }
    bit_stack.push(deci_number % 2);
    bit_stack.push(1);
    std::string ret = "";
    while (!bit_stack.empty()) {
        if (bit_stack.top() == 1) ret.push_back('1');
        else ret.push_back('0');
        bit_stack.pop();
    }
    return ret;
}

std::string binary_to_hexadecimal(const std::string& bina_string)
{
    std::string ret;
    std::string cp_bina_string(bina_string);
    if (cp_bina_string.size() % 4) {
        std::reverse(cp_bina_string.begin(), cp_bina_string.end());
        while (cp_bina_string.size() % 4) cp_bina_string.push_back('0');
        std::reverse(cp_bina_string.begin(), cp_bina_string.end());
    }
    std::string hex_bit;
    ret += "0x";
    for (int i = 0, len = cp_bina_string.size(); i < len; i += 4) {
        hex_bit = cp_bina_string.substr(i, 4);
        ret.push_back(dict_bin_to_hex[hex_bit]);
    }
    return ret;
}

void show_bitpatterns(int decimal_value)
{
    char sign_bit = '0';
    std::string expo_bits;
    std::string frac_bits;
    const int BIAS = 127;

    std::cout << "Decimal Form: " << decimal_value << std::endl;

    std::string decimal_string = std::to_string(decimal_value);
    std::string binary_string = decimal_to_binary(decimal_string);

    std::string cp_binary_string(binary_string);

    std::reverse(binary_string.begin(), binary_string.end());
    while (binary_string.size() < 32) binary_string.push_back('0');
    std::reverse(binary_string.begin(), binary_string.end());
    std::cout << "Integer Bitpattern: " << binary_string << std::endl;
    std::cout << "Hexa Form: " << binary_to_hexadecimal(binary_string) << std::endl;

    std::string float_bitpattern = "";
    int Exp = cp_binary_string.size() - 1;
    int expo_value = Exp + BIAS;
    expo_bits = decimal_to_binary(std::to_string(expo_value));
    frac_bits = cp_binary_string.substr(1, cp_binary_string.size() - 1);
    while (frac_bits.size() < 23) frac_bits.push_back('0');
    float_bitpattern.push_back(sign_bit);
    float_bitpattern += expo_bits;
    float_bitpattern += frac_bits;
    std::cout << "Float Bitpattern: " << float_bitpattern << std::endl;
    std::cout << "Hexa Form: " << binary_to_hexadecimal(float_bitpattern) << std::endl;   
}

// Output:
// Enter unsigned number: 12345
// Decimal Form: 12345
// Integer Bitpattern: 00000000000000000011000000111001
// Hexa Form: 0x00003039
// Float Bitpattern: 01000110010000001110010000000000
// Hexa Form: 0x4640E400