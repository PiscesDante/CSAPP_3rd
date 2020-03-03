#include <map>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stack>

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

std::string binary_to_hexadecimal(const std::string& bina_string);
std::string hexadecimal_to_binary(const std::string& hexa_string);
std::string binary_to_decimal(const std::string& bina_string);
std::string decimal_to_binary(const std::string& deci_string);
std::string hexadecimal_to_decimal(const std::string& hexa_string);
std::string decimal_to_hexadecimal(const std::string& deci_string);

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

std::string hexadecimal_to_binary(const std::string& hexa_string)
{
    std::string ret = "";
    std::map<char, std::string> dict_hex_to_bin;
    for (auto item : dict_bin_to_hex) dict_hex_to_bin[item.second] = item.first;
    size_t len = hexa_string.size();
    for (size_t i = 2; i < len; ++i) ret += dict_hex_to_bin[hexa_string[i]];
    return ret;
}

std::string binary_to_decimal(const std::string& bina_string)
{
    int ret = 0;
    size_t len = bina_string.size();
    std::string cp_bina_string(bina_string);
    std::reverse(cp_bina_string.begin(), cp_bina_string.end());
    for (int i = 0, base = 1; i < len; ++i, base *= 2) {
        if (cp_bina_string[i] == '1') ret += base;
    }
    return std::to_string(ret);
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

std::string hexadecimal_to_decimal(const std::string& hexa_string)
{
    std::string binary_form = hexadecimal_to_binary(hexa_string);
    return binary_to_decimal(binary_form);
}

std::string decimal_to_hexadecimal(const std::string& deci_string)
{
    std::string binary_form = decimal_to_binary(deci_string);
    return binary_to_hexadecimal(binary_form);
}