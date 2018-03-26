#include <stdio.h>

typedef void (*f)();

// I don't know whether the passed arguments would be "padded" or not,
// also, I don't know how to handle with so called "data structure alignment"
// but this code can run without any bugs on my computer :)
// so I think I can learn about no matter "padding" or "alignment" some days later ;)
// and plz do not tease me for my poor english ;(

int call(f pf, ...) {
    // usage: (return_type) call((f) function_name [, arg1, arg2, ...])
    // plz use gcc!
    int return_to;
    __asm__ __volatile__ (
        // 伪装为从未进入这个函数，直接由上一函数调用 *pf
        "movl %%ebp, %%esp;"  // 重设栈基
        "movl (%%esp), %%ebp;"  // 恢复上一个函数的栈基
        "movl 4(%%esp), %0;"
        "movl %0, 8(%%esp);"  // 舍弃第一个变量(pf)，用跳回点地址覆盖
        "addl $8, %%esp;"  // 重设栈顶
        "jmp *%1;"  // 正式跳转
        // *pf 执行完后，控制权直接回到上一函数，所以这里不必干涉返回值的传递
        : "=&r" (return_to)
        : "nr" (*pf));
    // this line would never be executed
}

int sum(int x, int y) { return x+y; }
int main() {
    printf("Returned: %d\n", (int) call( (f) sum, 1, 2 ));
    return 0;
}
