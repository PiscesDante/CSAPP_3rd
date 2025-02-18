# Lecture 8. Machine Level Programming IV: Data

我们今天将讨论数据的表示。目前为止，我们看到过的所有程序都仅仅只是操作整数或者指针。这就是所谓的「标量数据」，特征就是不存在聚合的现象。而今天将看到把数据收集起来的情况，即将多个数据元素放在一起。两种方法可以将数据放在一起：

* 使用数组，你可以通过数组创建许多相同数据类型的副本；
* 使用结构，这是一个可以包含不同数据类型的结构，可以通过标签或者名称访问其中不同的数据成员；

机器级代码中是没有「数组」这一高级概念的。而是将其视为字节的集合。存储在连续内存上的区块。

## 今日话题

* 数组：
    * 一维数组；
    * 多维数组：聚合的；
    * 多层的数组；
* 结构：
    * 分配；
    * 访问；
    * 队列；
* 浮点类型；

## 数组的分配

* 基本原则： `Typename ArrayName[Length];` 
    * 关于数据类型 `Typename` 长度为 `Length` 的数组 `ArrayName` ；
    * 在内存中以物理连续的形式分配 `Length * sizeof(Typename)` 字节的内存区块；这段内存都会用来保存该数组中的内容。

数组的寻址都是使用内存区域开头的歧视地址，然后加上一个偏置值来完成寻址的。

## 数组的访问

* 基本原则： `Typename ArrayName[Length];`
    * 标识符 `ArrayName` 可以用作一个指向第一个元素的指针；

在数组越界的情况下，可能得到的数据是无效的，甚至得到的地址都是无效的( Segmentation Fault )。

## 数组案例

```C
#define ZLEN 5 /* Do not use magic number */
typedef int zip_dig[ZELN];

zip_dig cmu = { 1, 5, 2, 1, 3 };
/* 16 | 20 | 24 | 28 | 32 | 36 */
zip_dig mit = { 0, 2, 1, 3, 9 };
/* 36 | 40 | 44 | 48 | 52 | 56 */
zip_dig ucb = { 9, 4, 7, 2, 0 };
/* 56 | 60 | 64 | 68 | 72 | 76 */
```

* 这种声明方式等同于 `int[5] cmu` ；
* 每个数组分配了 `5 * sizeof(int) = 20` 个字节；

不要使用魔数，也就是没有任何文本说明的字面数值； C 语言中应当将本应该是魔数的数值变成宏定义。这种宏定义一般在头文件中。

以上数组中所有的元素在内存中是物理意义上的连续。

指针解引用的方式是由内向外解读。

## 多维「聚合的」数组

* 多维数组的声明： `Typename ArrayName[R][C];` 
    * 类型为 `Typename` 的二维数组；
    * `R` 行， `C` 列；
* 数组的大小： `R * C * sizeof(Typename)` ；
* 在内存中的存储方式： **<u>行优先方式</u>** ；也就是以行为元素存储在内存这个「数组」中。

一个数组包含 5 个元素，而每个元素是包含 5 个 `int` 的数组。

## 多维数组的寻址方式

* 假设 `A[i]` 是一个拥有 `C` 个元素的数组；
* 每个元素有 `K` 个字节；
* 那么定位第 `i` 个元素（也就是这个数组）的地址方式为： `A + i * (C * K)` ；
* 如果要寻址 `A[i][j]` 这个特定的元素：
    * 那么公式为： `A + i * (C * K) + j * K` ；
    * 化简一下： `A + (i * C + j) * K` ；

## 多维数组的元素访问

那么对于多维数组的寻址方式，将基本上采用索引值，并且使用内存引用，移位以及 `LEA` 指令等的某种组合来实现地址的计算，从而对元素实现定位。

### 地址的计算

```C
int get_univ_digit(size_t index, size_t digit)
{
    return univ[index][digit];
}
```

* 元素的地址计算： `Mem[Mem[univ + 8 * index] + 4 * digit]` ；
* 在定位过程中必须读取两次内存：
    * 第一次是获得元素所在行的行首地址；
    * 第二次是利用这个行首地址加上偏置值之后获得元素地址

## 结构的表现形式

```C
struct rec {
    int a[4];
    size_t i;
    struct rec *next;
};
```

* 结构在内存中以内存区块的形式存在：这个内存区块必须能够容纳所有的字段；
* 字段在内存中的存储顺序按照声明的顺序排列：即使其他的排列方式更加节省内存；
* 编译器决定结构的大小以及字段的位置：在机器级编程中没有结构这个概念；

## 结构 & 对齐

* 未对齐的数据：

    ```
    |c| i[0] | i[1] | v       |
    p p+1    p+5    p+9       p+17
    ```

* 对齐的数据：若原始数据需要 `K` 字节的内存空间，那么计算机更青睐这个数据的起始地址是 `K` 的倍数。

    ```
    |c| 3 Bytes | i[0] | i[1] | 4 Btyes | v       |
    p p+1       p+4    p+8    p+12      p+16      p+24
    ```

**<u>也就是当一个结构存储在内存中时，编译器实际上会在分配空间时，在数据结构中插入一些空白的，不被使用的字节，这么做只为了保持「对齐」</u>** 。

## 对齐的原则

* 对齐的数据

    * 原始数据若需要 `K` 个字节；
    * 地址就必须是 `K` 的整数倍；
    * 这个规则在一些机器上是必须的，但是在 `x86-64` 上是被建议的；

* 对齐数据的动机

    * **<u>在一些系统中，内存必须是以 4 或者 8 字节为一块来访问；也就是内存系统，实际的硬件内存一次不取一个字节，实际上现在大多数的机器，一次取大约 64 个字节。一般来说，如果因为一个没有对齐的地址，一个特定的数据跨越了两个块之间的边界，这就会让硬件，甚至有可能让操作系统来采取一些额外的步骤来处理这个问题。因此，仅仅是出于效率原因，才使用的数据对齐</u>** 。

        在 `x86-64` 的机器上，如果内存数据没有进行对齐，那么仅仅是运行速度慢一点，而其他的机器上，可能会导致内存错误。

## 结构的数组



## 节省空间

因为在对齐的过程中会发生空间浪费的情况，那么可不可以让编译器不要做对齐的操作？比起禁止编译器做对齐， **<u>更好地方法是声明字段时，将类型长度较大的字段放在前面</u>** 。这样就可以保证编译器不会浪费太多的空间。

