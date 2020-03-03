#include <cstdio>
#include <iostream>

void show_bytes(unsigned char* ch, size_t len)
{
    for (int i = 0; i < len; ++i) printf("%.2x ", ch[i]);
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    float fval = 12345;
    int ival = 12345;
    std::cout << "float value : 12345" << std::endl;
    show_bytes((unsigned char*)(&fval), sizeof(fval));
    std::cout << "int value : 12345" << std::endl;
    show_bytes((unsigned char*)(&ival), sizeof(ival));
    /**
     * float value : 12345
     * 00 e4 40 46 -> 0x4640E400
     * int value : 12345
     * 39 30 00 00 -> 0x00003039
     * 这里可以看出，虽然数值都是 12345
     * 但是 float 和 int 对这个数值的编码方式完全不同
     */
    return 0;
}