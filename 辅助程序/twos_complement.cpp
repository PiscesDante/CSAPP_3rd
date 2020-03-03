#include <cstdio>

void show_bytes(unsigned char* chs, size_t len)
{
    for (int i = 0; i < len; ++i) {
        printf("%.2x ", chs[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    short x = 12345;
    short mx = -12345;
    // short类型占两个字节
    show_bytes((unsigned char*)(&x), sizeof(x));   // 39 30 -> 0x3039
    show_bytes((unsigned char*)(&mx), sizeof(mx)); // c7 cf -> 0xCFC7
    return 0;
}

/**
 * 使用进制转换器转换成二进制之后，可以看到
 * 负数的表示就是 正数的 各位取反（反码）之后加一
 * 当然负数取正数也可以这么操作
 * postiveNumber = HexadecimalToBinary("0x3039")
 * negativeNumber = HexadecimalToBinary("0xCFC7")
 * print(postiveNumber)  # 0011000000111001
 * ********************  # 1100111111000110 -> 反码
 * print(negativeNumber) # 1100111111000111 -> 补码表示的负数 反码加上1
*/