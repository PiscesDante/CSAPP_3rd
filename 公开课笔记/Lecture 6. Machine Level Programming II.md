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

有两种跳转方式：「无条件跳转」和「条件跳转」。有条件跳转的条件就是条件码寄存器中的内容。

## 条件分支案例（旧）

```C
long absdiff(long x, long y)
{
    long result;
    if (x > y) {
        result = x - y;
    } else {
        result = y - x;
    }
    return result;
}
```

```ASM
#            x ---> %rdi
#            y ---> %rsi
# return value ---> %rax

absdiff:
	cmpq %rsi, %rdi # x:y 
	jle .L4
	movq %rdi, %rax
	subq %rsi, %rax
	ret
.L4: # x <= y
	movq %rsi, %rax
	subq %rdi, %rax
	ret
```

## 使用 Goto 语句来解释以上汇编语言

* C 语言允许使用 `goto` 语句；
* 该语句可以跳转到指定的标签处；

```C
long absdiff_j(long x, long y)
{
    long result;
    int ntest = x <= y;
    if (ntest) goto Else;
    result = x - y;
    goto Done;
Else:
    result = y - x;
Done:
    return result;
}
```

## 使用条件移动（一种优化手段）

这个优化的基本思想是 **<u>我把 `then` 代码和 `else` 代码都执行，并且得到两个结果，然后才会选择使用哪个结果</u>**。 看起来多此一举，但是实际上非常有效率。

当程序运行到一个分支的时候，处理器就开始猜测到底该执行哪一个分支，这时就开始猜测。一般情况下都会猜对（这就非常有效率），但是也会有猜错的时候（一旦猜错，那就必须转向另一个分支并且重新开始，较差的情况下，可能会使用 40 个指令和 40 个时钟周期）。所以，这个优化的手段就是，先两个都执行，这样就可以减小预测错误的代价，从而迅速的掉头执行另一个分支的指令。

* 条件移动指令
    * 指令支持： `if(Test) Dest <- Src` ；
    * 受到 post - 1995 x86 处理器的支持；
    * GCC 尝试使用这个优化，但是仅仅在明确知道这个操作是安全的情况下；
* 为什么？
    * 分支对于管线中的指令流是具有破坏性的（流水线技术：这意味着在完成一个指令的执行之前，就已经开始执行下一个指令，并且流水线的深度可能达到 20 条指令）；
    * 条件移动不需要控制权的转移；

当编译器使用条件移动技术的时候，上面的函数会变成下面的样子：

```ASM
absdiff:
	movq %rdi, %rax   # x
	subq %rsi, %rax   # result = x - y
	movq %rsi, %rdx
	subq %rdi, %rdx   # eval = y - x
	cmpq %rsi, %rdi   # x:y
	cmovle %rdx, %rax # if <=, result = eval
	ret
```

## 使用「条件移动」的糟糕结果

为什么不能滥用「条件移动」？原因有两个：

* 进行两种计算可能是非常糟糕的做法，如果你不得不在一个分支中破解密码（计算素数或者进行计算量很大的工作），这样就会产生巨大开销，GCC 的策略是，只有在所有分支都是简单计算的情况下才会使用「条件移动」优化。

    ```C
    val = Test(x) ? HardWork1(x) : HardWork2(x);
    ```

* 另外一种情况可能是单纯无法使用该优化。比方说一个分支中只有一个空指针表达式：

    ```C
    val = p ? *p : 0;
    ```

    这种情况不可能对空指针进行解引用。

* 或者还有一种情况是，所有分支都是能改变程序状态的代码，这样也不能同时进行执行。

故，条件移动仅仅应当在 **<u>两个计算相对安全，并且没有副作用的情况下使用</u>** 。

## 「Do-While」循环案例

```C
/* C Code */
/* Count how many 1 are in the bitpattern */
long pcout_do(unsigned long x)
{
    long result = 0;
    do {
        result += x & 0x1;
        x >>= 1;
    } while (x);
    return result;
}
```

```C
/* Goto Version */
long pcout_do(unsigned long x)
{
    long result = 0;
Loop:
    result += x & 0x1;
    x >>= 1;
    if (x) goto Loop;
    return result;
}
```

## 普通的「While」循环解释 #1

```C
/* While Version */
while (Test) {
    Body
}

/* Goto Version */
goto Test;
Loop:
	Body
Test:
	if (Test) {
        goto Loop;
    }
Done:
	...
```

## While 循环的案例 #1

```C
/* C Code */
long pcount_while(unsigned long x)
{
    long result = 0;
    while (x) {
        result += x & 0x1;
        x >>= 1;
    }
    return result;
}
```

```C
/* Jump to Middle */
long pcount_goto_jtm(unsigned long x)
{
    long result = 0;
    goto Test;
Loop:
    result += x & 0x1;
    x >>= 1;
Test:
    if (x) goto Loop;
    return result;
}
```

## 「For」循环形式

```C
#define WSIZE 8 * sizeof(int)

long pcount_for(unsigned long x)
{
    size_t i;
    long result = 0;
    for (i = 0; i < WSIZE; ++i) {
        unsigned bit = (x >> 1) & 0x1;
        result += bit;
    }
    return result;
}
```

## Switch 语句案例

```C
long switch_eg(long x, long y, long z)
{
    long w = 1;
    switch(x) {
        /* case must be constant integer value */
        case 1: {
            w = y * z;
        } break;
        case 2: {
            w = y / z; /* if case is 2 */
        } /* Fall Through */
        case 3: {
            w += z;/* if case is 2 & 3 */
        } break;
        case 5:
        case 6: {
            w -= z; /* if case is 5 & 6 */
        } break;
        default: {
            w = 2;
        }
    }
    return w;
}
```

* 多重标签： 5 & 6；
* 穿透标签： 2；
* 丢失标签： 4；

如果你没有在 `case` 的末尾加上 `break` 语句，它就会一直执行下去。这是编程语言历史上最糟糕的设计之一。

可以将多个 `case` 放在一起（如果你认为这只是一个特殊情况，需要继续往下执行的话）。

如果没有标签被匹配成功（ 在当前的案例中， `x = 4` ），那么就会执行 `default` 分支。

## 跳转表的结构

跳转表的基本结构就是一个存储着内存地址的数组，内存地址指向需要执行的指令。

## Switch 语句案例分析

```C
long switch_eg(long x, long y, long z)
{
    long w = 1;
    switch(x) {
        ...
    }
    return w;
}
```

```ASM
switch_eg:
	movq %rdx, %rcx
	cmpq $6, %rdi # x:6
	ja .L8 # jump above (unsigned)
	jmp *.L4(, %rdi, 8)
```

这里 `ja` 指令的使用非常机智： **<u>若 `x` 大于 6 或者小于 0 的情况下会发生跳转，也就是说，将补码以原码的方式进行解释之后，小于 0 的补码还是会远大于 6 ，所以使用 `ja` 是没有问题的</u>** 。

`jmp *.L4(, %rdi, 8)` 指令是 Switch 语句的核心，这条指令 **<u>是将表中的数据进行索引并从中提取出一个地址，然后跳转到这个提取出的地址上</u>** 。这就是能够直接进入某些代码块的原理。

跳转表会为不同的情况提供相同的跳转地址。

## 处理标签穿透

```C
long w = 1;
...
case 2: --------------> if case 2: w = y / z; goto merge
	w = y / z;
case 3: --------------> if case 3: w = 1; merge: w += z;
	w += z;
...
```

编译器有时候会作出很匪夷所思的动作：比方说现在， `w` 在 Switch 代码块中有赋值的动作，所以编译器就将 `long w = 1` 这个动作暂时不执行。当 `case` 是 2 的时候，执行赋值，并且跳转到 `merge` 继续执行 `w += z` 。但是如果 `case` 为 3 的时候，编译器就会先执行最开始的 `long w = 1` 然后再执行 `merge` 。

* 如果 `case` 的值是个负数怎么办？答： **<u>通常会通过给每一项增加一个「偏置值」来使最小值变成 0</u>** 。

* 如果 `case` 的值只有 0 和 1000000 两种，难道中间的所有值都会建表吗？答： **<u>这种情况会变成 `if-else`</u>** 。

虽然 Switch 语句不是很常用，但是还是一种很有效的解决方案。在 Switch 语句中获取所需位置的时间复杂度为 $O(1)$ 。加上查找的时间，基本上可以在对数时间内完成。不管哪种情况下，Switch 语句的性能是优于 IF-ELSE 的。

编译器会自动的设置跳转树。
