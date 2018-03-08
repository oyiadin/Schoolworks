#include <stdio.h>

#define lambda(type, function_body) \
    ({ type fn function_body fn; })
// usage: lambda(int, (int a, int b) { return a+b; })(2, 3) == 5
// 涉及的第一个特性叫 Statement Expression [1]
// 由括号包裹起来的代码可以有自己的循环、分支甚至是局部变量
// 考虑到这里涉及的代码不止一行，所以用花括号包裹起来(这一项是标准 C)
// 最后一行代码将成为整个 Statement Expression 的值
// 展开得到：
// ({
//   int fn (int a, int b) { return a+b; }
//   fn;
// }) (2, 3);
// 首先在 Statement Expression 中定义了一个局部函数，然后返回这个函数并调用
// 涉及到第二个特性叫 Nested Functions，可以在函数内定义函数 [2]

#define call(type, f, ...)              \ // 变参宏，C99
    lambda(type, (void(*pf)(), ...) {   \ // 传递的参数无需做处理，告诉编译器需要变参支持即可
        int return_to;                  \ // 储存调用点下一行指令的地址
        __asm__ __volatile__ (          \ // 内嵌汇编
                                        \ // 此时栈已经被清空重设，需要恢复未调用时的状态
            "movl %%ebp, %%esp;"        \ // 恢复栈顶
            "movl (%%esp), %%ebp;"      \ // 恢复栈基
            "movl 4(%%esp), %0;"        \ // 保存回溯点
            "movl %0, 8(%%esp);"        \ // 回溯点往上挪，覆盖掉传进来的第一个参数(pf)
            "addl $8, %%esp;"           \ // 使栈顶所指符合stdcall约定
                                        \ // 栈状态已经伪装完毕，仿佛未曾来过此地
                                        \ // 覆盖掉了 pf 这个额外的参数，其他参数原样扔给被调用函数
            "jmp *%1;"                  \ // 正式跳到被调用函数
            : "=&r" (return_to)         \
            : "nr" (*pf));              \
    })((void(*)()) f, __VA_ARGS__)

int sum(int x, int y) { return x+y; }
int main() {
    printf("Returned: %d\n", call(int, sum, 1, 2));
}
