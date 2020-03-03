#include "./number_form_converter.hpp"

int main(int argc, char* argv[])
{
    char is_continue;
    do {
        std::cout << "Number Form ( dec/bin/hex ) : ";
        std::string form;
        std::cin >> form;
        std::cout << "Number Value: ";
        std::string value;
        std::cin >> value;
        if (form == "dec") {
            std::cout << "Bin form: " << decimal_to_binary(value) << std::endl;
            std::cout << "Hex form: " << decimal_to_hexadecimal(value) << std::endl;
        } else if (form == "bin") {
            std::cout << "Dec form: " << binary_to_decimal(value) << std::endl;
            std::cout << "Hex form: " << binary_to_hexadecimal(value) << std::endl;
        } else if (form == "hex") {
            std::cout << "Bin form: " << hexadecimal_to_binary(value) << std::endl;
            std::cout << "Dec form: " << hexadecimal_to_decimal(value) << std::endl;
        } else {
            std::cerr << "Input Error." << std::endl;
        }
        std::cout << "Continue? [y]/n: ";
        is_continue = 'y';
        std::cin >> is_continue;
    } while (is_continue == 'y' || is_continue == 'Y');
    std::cout << "Bye~" << std::endl;
    return 0;
}