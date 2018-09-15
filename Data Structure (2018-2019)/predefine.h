#ifndef PREDEFINE_H
#define PREDEFINE_H

#include <cassert>
#include <cmath>
// abs, floor 之类的我就懒得引过去了，直接拿来用

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
// #define INFEASIBLE -1
// #define OVERFLOW -2
// 跟 cmath 冲突了…

typedef int Status;

#ifndef NO_DEFAULT_TYPES
typedef int ElemType;
// 教材要是选用 C++ 作为语言就没这么多麻烦事了
#endif

typedef int (*_comp_func)(ElemType, ElemType);
int _comp_max(ElemType a, ElemType b) { return a - b; }
int _comp_min(ElemType a, ElemType b) { return b - a; }

#ifdef USE_C_MAX_MIN
// C 写法的变参 max/min
// 默认使用 C++ 写法

#include <cstdarg>
#define get_args_count_n(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define get_args_count(...) get_args_count_n(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
// 可以得到变参宏实际传入的参数个数，思路从网上找到的，很牛批
ElemType _func_comp(int n, _comp_func comp, ...) {
  va_list vl;
  va_start(vl, n);

  ElemType largest = va_arg(vl, ElemType);
  for (int i = 1; i < n; ++i) {
    ElemType new_value = va_arg(vl, ElemType);
    largest = (comp(largest, new_value) > 0) ? new_value : largest;
  }

  va_end(vl);
  return largest;
}
#define max(...) \
  _func_comp(get_args_count(__VA_ARGS__), _comp_max, __VA_ARGS__)
#define min(...) \
  _func_comp(get_args_count(__VA_ARGS__), _comp_min __VA_ARGS__)

#else
// C++11 写法的变参 max/min

#include <initializer_list>
template<typename T>
T _func_comp(_comp_func comp, std::initializer_list<T> args) {
  assert(args.size());
  T largest = *args.begin();
  for (auto &i : args)
    if (comp(i, largest) > 0)
      largest = i;
  return largest;
}
#define max(...) _func_comp(_comp_max, {__VA_ARGS__})
#define min(...) _func_comp(_comp_min, {__VA_ARGS__})
// 算是我自定义的语法糖吧 233
// 懒得手动加上花括号了，刚好能跟上边 C 的版本兼容

#endif

#endif