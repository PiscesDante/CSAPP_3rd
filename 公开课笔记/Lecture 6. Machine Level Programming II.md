# Lecture 6. Machine Level Programming II: Control

 ## 今天的内容

* 控制：若干种条件码；
* 条件分支；
* 循环；
* `Switch` 选择语句；

## 处理器状态（ `x86-64` ，部分 ）

### 当前正在执行的程序的信息存放情况

* 临时的数据： `%rax` , ... ；
* 运行时栈的位置： `%rsp` ，这个寄存器是「栈指针」，这个寄存器基本上是程序运行中最重要的寄存器；
* 运行时指令位置： `%rip ` , ... 这个寄存器存储的内容是当前正在执行的指令的地址，告诉用户当前的程序执行到哪里了；
* 最近的状态码： （ CF，ZF，SF，OF ）；

## 条件码（隐式设置）

### 只有 1 位的寄存器

* `CF` ： Carry Flag（无符号整型），两个无符号整数相加，如果最高位产生进位，那么该寄存器置 `1` 。这个寄存是用来探查无符号整数是否溢出的条件码；
* `ZF` ： Zero Flag，当两个操作数的计算结果为 `0` 时就会置 `1` ；
* `SF` ： Sign Flag（有符号整型）；
* `OF` ： Overflow Flag（有符号整型），这个寄存器是判断补码是否溢出的证据，如果为 `1` 那么就是产生了溢出；

### 以上条件码寄存器是被算术操作隐式赋值的

**<u>以上的条件码寄存器是由算术操作隐式赋值的，可以认为寄存器的赋值是算术操作的一个副作用</u>** 。

```
Example : addq src, dst <---> t = a + b

* CF set if carry out from most significant bit(unsgined overflow);
* ZF set if (t == 0);
* SF set if (t < 0) as signed;
* OF set if two's-complement(signed) overflow;
```

### 以上的条件码不会被 `leaq` 指令所设置

## 条件码（显式设置：比较操作）

### 以下条件码寄存器会被比较操作显式的进行设置

`cmp` 指令和减法指令几乎一样，但是 `cmp` 指令不会对得到的结果作出任何操作；也就是少了赋值那一步。 **<u>仅仅是对两个值做减法，不会对结果做任何操作</u>** 。但是这个操作会设置四个条件标志：

```
cmpq src2, src1
[cmpq b, a] like computing a-b without setting destination

* CF set if carry out from most significant bit(unsgined overflow);
* ZF set if (a-b == 0);
* SF set if (a-b < 0) as signed;
* OF set if two's-complement(signed) overflow;
/* Same as the condition of arithmetic */
```

## 条件码（ 显式设置： `Test` ）

### 测试指令会对条件码寄存器进行显式的设置

`testq b, a` 指令就像计算 `a & b` 但是不对目标寄存器进行设置。

* 条件码寄存器的设置取决于 `a & b` 的结果；
* 在做掩码运算时非常有用(可以将一个数字作为条件来设置条件码寄存器)；

```
testq b, a

* ZF set if (a&b == 0);
* SF set if (a&b < 0);
```

## 读取条件码

可以直接从这个特殊的状态寄存器中提取获得条件码的值。但是一般不推荐这样做。但是我们可以使用 `set` 指令来对通用寄存器的最低字节进行「根据条件码」的设置。

注意： `set` 指令不会影响到寄存器中其他高位字节的内容，仅仅设置寄存器中最低位字节。

对于 16 个普通寄存器，我们可以直接将其最低位的那个字节设置成 `0` 或者 `1` 而不会影响其他的字节。

## 读取条件码（并以其为条件来设置寄存器中的字节）

* `SetX` 指令：将一个最低字节根据条件码的组合来进行设置；
* 对于任何一个可寻址字节的寄存器来说：
    * 这个指令并不会改变其他的高位字节；
    * 一般来说这个指令会配套使用 `movzbl` 来完成工作: 该指令会将剩余的位设置为 0 ； 

```C
int gt(long x, long y)
{
    return x > y;
}
/* Argument x ---> %rdi */
/* Argument y ---> %rsi */
/* Argument z ---> %rax */
```

```ASM
cmpq   %rsi, %rdi # Compare x:y
setg   %al        # Set the first byte as 00000001 when > 
# and 00000000 when > does not work
movzbl %al, %eax  # Zero rest of %rax
# Turns out that the %rax's Low 32-bits is 000...00000001
# so we got value 1 for TRUE
ret
```

但是从上面的汇编代码可以确定的是，我们仅仅设置了低 32 位，那么如何保证 `%rax` 的高 32 位同样是 0 ？

这就是 `x86-64` 一个非常奇怪的地方了： **<u>任何计算都会获得 32 位的结果，这个性质会将高 32 位全部设置为 0</u>** 。这是 AMD 设计的。

但是更加常见的操作是，程序员不仅仅想用这个条件码去更改寄存器中的一个字节。而是想通过这个条件码来作出一些操作。

## 跳转

