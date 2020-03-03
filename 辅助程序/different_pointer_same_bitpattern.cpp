#include <cstdio>
#include <iostream>

using byte_pointer = unsigned char*;

void show_bytes(byte_pointer start, size_t len)
{
    for (size_t i = 0; i < len; ++i) printf("%.2x ", start[i]);
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    unsigned int x = 12345; // 分配内存，以 int 类型编码来存储数值 12345
    show_bytes((byte_pointer)(&x), sizeof(x)); // 显示底层的序列
    unsigned char* ch = (byte_pointer)(&x); // 转换为字符指针
    std::cout << *ch << std::endl; // 输出字符指针解释的位序列
    show_bytes((byte_pointer)((float*)(&x)), sizeof(x)); // 显示底层位序列
    float* fl = (float*)(&x); // 转换为单精度浮点数指针
    std::cout << *fl << std::endl; // 由单精度浮点数指针来解释 11000000111001 是什么含义
    /**
     * 39 30 00 00
     * 9 -> 当使用 unsigned char指针 解释最低字节 00111001 就会变成 '9' 这个字符
     * 39 30 00 00
     * 1.7299e-41 -> 当 float指针 来解释这串字节序列就会变成这个数值
     * 这里可以看到最低有效位保存在最前端
     * 当前环境（Windows）采用的是 ·小端法· 保存数据
     * 而且无论指针怎么变换，保存在内存中的位序列是不会变化的
     * 显示不同数值的原因仅仅是 ·不同类型指针以自己的方式解释了这个比特序列·
     * 不同的指针会对相同的位序列解释出不同的含义
     */

    /**
     * >>> import NumberConverter
     * >>> NumberConverter.DecimalToBinary('12345')
     * '11000000111001' 12345 的 二进制 表示
     * >>> NumberConverter.BinaryToHexadecimal('11000000111001')
     * '0x3039' 12345 的 十六进制 表示
     */
    return 0;
}