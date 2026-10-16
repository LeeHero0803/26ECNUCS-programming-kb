# 有种宝典：基础 IO 篇（一）

> 本篇只讲 **输入**，主要是覆盖常见 OJ 题目的输入格式。

## 1. 首先要理解：空格和换行通常都是分隔符

例如输入：

``` text
// 情况一：空格分割
10 20 30

// 情况二：换行分割
10
20
30
```

如果需要依次读取三个整数，`scanf` 和 `cin >>`
都会自动跳过空格、换行、制表符等**空白字符**。

``` cpp
// c语言
int a, b, c;
scanf("%d%d%d", &a, &b, &c);

// c++
int a, b, c;
cin >> a >> b >> c;

// 上面两种都可以得到 a = 10, b = 20, c = 30
```

这类输入通常不关心整数之间是空格还是换行。
只有题目要求按 **"整行"** 处理时，才需要特别关注行边界。

## 2. 输入固定数量的数据

### 输入给定数量的数组

例如输入，第一行是要输入的整数个数 `n`，第二行是用空格分开的 `n` 个整数：

```text
6
1 2 3 4 5 6
```

对于 c 语言，一般使用 malloc/free 动态内存来控制数组长度

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    scanf("%d", &n);

    // 堆上分配n个int空间，注意单位字节 sizeof(int)
    int *a = (int *)malloc(n * sizeof(int));
    if (a == NULL) { // 内存分配失败判断，好习惯
        return 1;
    }

    // 读入n个数字
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    // 使用数组 a

    free(a); // 用完释放，必须写，防止内存泄漏
    a = NULL; // 置空野指针，可选好习惯
    return 0;
}
```

对于 c++，可以使用 vector，也可以使用 new 动态堆内存进行手动初始化与释放。

``` cpp
#include <iostream>
#include <vector>
using namespace std;
int main(){
    int n;
    cin >> n;
    vector<int> a(n, 0); // 开辟n个int，全部初始化为0
    for(int i=0;i<n;i++){
        cin >> a[i];
    }
    return 0;
}
```

```cpp
#include <iostream>
using namespace std;
int main(){
    int n;
    cin >> n;
    int *a = new int[n]{0}; // 堆上分配
    for(int i=0;i<n;i++){
        cin >> a[i];
    }
    delete[] a; // 必须释放，防止内存泄漏
    return 0;
}
```

除以上方法外，如果题目已知数据上限，也可以把数组 a 在一开始就设定一个很大的容量。以下代码对 c/c++ 都成立。

```cpp
// c语言
#include <stdio.h>
int main(void) {
    int n;
    scanf("%d", &n);
    static int a[10005] = {0}; // static局部，放在静态存储区

    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    return 0;
}

// c++语言
#include <iostream>
using namespace std;
const int MAXN = 10005;
int a[MAXN] = {0}; // 全局数组自动初始化为0
int main(){
    int n;
    cin >> n;
    for(int i=0;i<n;i++){
        cin >> a[i];
    }
    return 0;
}
```

### 输入矩阵

已知矩阵有 `r` 行、`c` 列。

``` cpp
// c语言
int r, c, a[105][105];
scanf("%d%d", &r, &c);

for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
        scanf("%d", &a[i][j]);
    }
}
```

### 输入直到 EOF

有些题目不告诉你数据组数，而是要求一直读到文件结束（EOF，End Of File）

如，每次每行一个整数，直到 `EOF`：
```text
325
666
... {你不确定数目}
996
```

``` cpp
int x;
while (scanf("%d", &x) !+ EOF) {
    /* 处理 x */
}
// scanf 会返回成功读取到的变量数目
// 上面的代码就是说一直读到EOF为止

// 使用 cin >> 也是一样的
while (cin >> x) {
    // 处理 x
}
```

windows 中的 EOF 就是 **ctrl + z**

## 3. 数据数量不定

### 固定一行
如果题目明确要求"输入一行，有若干个整数，每个整数用空格分开，数量不固定"，就按行读取再解析。

```text
3 4 8 26 415 ... 46 2 5
```

``` c
// c语言
char buff[10005]; // 作为输入缓冲区
int a[10005] = { 0 };
fgets(buff, sizeof(buff), stdin);
int i = 0, used;
char *p = buff;
while (sscanf(p, "%d%n", &a[i], &used) == 1) { // 这里 %n 意思是，统计本次 sscanf 到目前为止，一共消耗了多少个字符，把这个字符数量写入变量 used
    /* 处理 */
    i++;
    p += used;
}
```

这里 `fgets` 读取一行，`sscanf` 从字符串中解析整数
这样，我们最后一共得到 `i` 个数据，存在 `a[0]` ~ `a[i - 1]` 里

``` cpp
// c++ 语言
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

string line;
getline(cin, line);
istringstream iss(line);
int a[10005], i = 0;
while (iss >> &a[i++]) {
    // 处理
}
```

使用字符串流对象

### 不确定多行

如果最后用 EOF 结束，那么就写成下面：

```cpp
// c语言
char buff[10005]; // 作为输入缓冲区
int a[10005] = { 0 };
int i = 0, used;
char *p = NULL;
while(fgets(buff, sizeof(buff), stdin) != NULL) {
    p = buff;
    while (sscanf(p, "%d%n", &a[i], &used) == 1) {
        /* 处理 */
        i++;
        p += used;
    }
}
 // c++ 语言
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

string line;
while(getline(cin, line)) {
    istringstream iss(line);
    int a[10005], i = 0;
    while (iss >> &a[i++]) {
        // 处理
    }
}
```

## 4. 读取整行文本

当输入包含空格的句子时，不能只用 `scanf("%s", ...)` 或 `cin >> s`，因为它们会在空白字符处停止。

例如输入：

``` text
hello world
```

``` c
// c 语言
char line[1000];

if (fgets(line, sizeof(line), stdin) != NULL) {
    /* line 保存读到的文本；若读到换行符，末尾可能包含 '\n' */
}
```

``` cpp
// c++ 语言
string line;
getline(cin, line);
```

两种方式都可以读取整行文本（注意最后的换行符）

> [!NOTE]
>
> 如果使用`scanf` / `cin >>`先读整数，再读一整行，前面的输入操作通常会留下行末换行符。需要先丢弃当前行剩余内容。
>
> 如：
>
> ```text
> 125
> Hello world!
> ```
>
> **C：**
>
> ``` c
> int n;
> scanf("%d", &n);
> getchar(); // 读取剩下的换行符
> // 读取下一行
> char line[1000];
> fgets(line, sizeof(line), stdin);
> ```
>
> **C++：**
>
> ``` cpp
> #include <iostream>
> #include <string>
> #include <limits>
> using namespace std;
> 
> int n;
> cin >> n;
> cin.ignore(); // 忽略缓冲区换行符
> // 读取一行
> string line;
> getline(cin, line);
> ```
>

## 5. 特殊格式输入

### 日期：`2025-03-08`

```cpp
// c语言
int year, month, day;
scanf("%d-%d-%d", &year, &month, &day);
// c++
int year, month, day;
char dash1, dash2;
cin >> year >> dash1 >> month >> dash2 >> day; // 人为去掉-
```

### 时间：`12:35:59`

``` cpp
// c语言
int h, m, s;
scanf("%d:%d:%d", &h, &m, &s);
// c++
int h, m, s;
char colon1, colon2;
cin >> h >> colon1 >> m >> colon2 >> s;
```

上面两个例子，相信大家都能看出来，用 `scanf` 会更加方便

### 各种进制的整数

如果输入 `FF` 或 `0xFF`，并希望按十六进制读取：

``` cpp
// c语言
unsigned int x;
scanf("%x", &x); // %x 用于读取十六进制整数，可识别带 0x 前缀的输入
// c++
cin >> hex >> x; // hex 会改变后续整数输入的进制状态。若之后还要按十进制读取，可以使用 dec 恢复
```

可以看出来用 scanf 会更加的方便
所以我们补充下面内容：

### scanf 常见格式控制符

> `scanf("格式串", 变量地址列表);`


**基础类型**
| 格式符 | 含义 | 变量类型 | 说明 |
| ---- | ---- | ---- | ---- |
| `%d` | 十进制有符号整数 | `int *` | OJ最常用 |
| `%u` | 十进制无符号整数 | `unsigned int *` | |
| `%ld` | long 十进制整数 | `long *` | C语言long |
| `%lld` | long long 十进制整数 | `long long *` | 64位整数，大数据必用 |
| `%hd` | short 短整型 | `short *` | |
| `%f` | 单精度浮点数 float | `float *` | |
| `%lf` | 双精度浮点数 double | `double *` | ⚠️ scanf读double必须`%lf`；printf输出double只用`%f` |
| `%c` | 单个字符 | `char *` | **不会自动跳过空格、换行**，会读取空白字符！ |
| `%s` | 字符串 | `char *` | 读到**空白（空格/换行/tab）就停止**；**不读空格**，不加长度限制有缓冲区溢出风险 |
| `%x` | 十六进制整数 | `int *` | 输入支持`0xff`或直接`ff`，大小写a/f均可，存成十进制数值 |
| `%o` | 八进制整数 | `int *` | |
| `%p` | 指针地址 | `void **` | OJ极少用到 |
| `%%` | 匹配字面的`%` | 无变量 | 用来读取输入里的百分号 |

**特殊：`%n`**
- `%n`：不读取任何输入字符
- 统计 从本次 scanf 开始到 %n 位置，一共成功读取了多少字符，写入对应的`int*`变量
- %n 不计入 scanf 返回值

```c
int used;
sscanf("123 45","%d%n", &x, &used);
// 读到123，used=3；返回值=1，不是2
```

**格式修饰符**（放在`%`和字母中间）
> `%[修饰符]类型符`

1. 数字（最大宽度）：`%5d`，最多读取5个字符。
    ```c
    int a;
    scanf("%3d", &a);
    // 输入12345，只会读123，剩下45留在缓冲区
    ```
2. `*` 星号：匹配但丢弃，不存入变量
    ```c
    int y,m,d;
    // 样例输入：2025/10/01
    scanf("%d%*c%d%*c%d", &y,&m,&d);
    // %*c 读取/，读完直接扔掉，不需要传变量地址
    ```
    可以用来处理分隔符，不用额外char变量存分隔符
3. 长度修饰符：
    - `l`：`%ld` long；`%lf` double
    - `ll`：`%lld` long long

**`%[]` 扫描集**（读取自定义字符集合）
`%[字符集合]`：只读取集合内的字符，遇到不在集合里的字符停止。

- `%[0123456789]` 只读取数字
- `%[a-zA-Z]` 只读取大小写字母
- `%[^\n]` —— 读到换行为止（可以读取带空格的一整行）
```c
char buf[100];
scanf("%[^\n]", buf); // 读取一行，包含空格，遇到\n停止
// 读完后换行符`\n`残留在输入缓冲区；下一次scanf会立刻终止
// 如需要继续读取需要用 getchar() 吃掉缓冲区内换行符
```

**字面字符**
格式串中不是%开头的普通字符，要求输入必须原样匹配
```c
int y,m,d;
scanf("%d-%d-%d", &y,&m,&d);
// 输入必须是类似 2026-10-15，中间的`-`要原样存在输入中
```
不匹配的话scanf直接终止
