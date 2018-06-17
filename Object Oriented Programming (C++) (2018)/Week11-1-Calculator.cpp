#include <map>
#include <cmath>
#include <cctype>
#include <string>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <sstream>

// 实现了：
// 1. 基本计算 (e * ** / % + -)
//    * 括号的处理会让代码逻辑变得混乱得多，所以还没实现左右括号
//    * 仅支持二元操作符
//    * 支持小数
// 2. 优先级
// 3. 错误回显：可指出出错位置、原因
// 4. 中间过程显示

#define DEBUG

#ifdef DEBUG
// 如果定义了 DEBUG 宏变量，打印中间过程
#define PRINT_PROCEDURE_IF_DEBUG                                         \
  if (op2char[op] != ".") {  /* 不打印“点号”运算符 */                      \
    std::cout << "... " << left << ' ' << op2char[op] << ' ' << right    \
              << " = " << stk_ods.peak() << std::endl;                   \
  }

#else
#define PRINT_PROCEDURE_IF_DEBUG void

#endif


class SyntaxException {
 public:
  SyntaxException(int a, const std::string m)
      : at(a), msg(m) {}
  void show(void) {
    std::cerr << "    ";
    for (int i = 0; i < at; ++i) std::cerr << ' ';
    std::cerr << '^' << std::endl
              << "=!> Syntax Error: " << msg << std::endl;
  }

 protected:
  const std::string msg;
  int at;
};


template <typename Tp>
class Stack {
 public:
  Stack() {}
  ~Stack();

  Tp &push(const Tp &elem);
  Tp pop(void);
  Tp &peak(void);

  inline bool is_empty(void);
  Stack &clear(void);

 protected:
  struct Node { Tp *elem;
                Node *prev, *next; };
  // 为了便于实现，采用双向链表
  // 元素 push() 时增长方向：head --> foot
  Node *head = 0, *foot = 0;
};

template <typename Tp>
Stack<Tp>::~Stack() {
  clear();
  // 析构时直接让 clear() 清空内容并回收内存即可
}

template <typename Tp>
Tp &Stack<Tp>::push(const Tp &elem) {
  Node *new_node = new Node;
  Tp *new_elem = new Tp;
  *new_elem = elem;
  new_node->elem = new_elem;
  // 将 elem 复制一份，新节点指向它
  // 注意：一个元素对应 new 了两次

  if (!foot) {  // 空链表
    new_node->prev = 0;
    new_node->next = 0;
    head = foot = new_node;
  } else {  // 链表非空
    foot->next = new_node;
    new_node->prev = foot;
    new_node->next = 0;
    foot = new_node;
  }

  return *new_elem;
}

template <typename Tp>
Tp Stack<Tp>::pop(void) {
  if (!foot)
    throw std::out_of_range("haven't any elements to be poped");

  Tp ret = *(foot->elem);
  delete foot->elem;
  Node *ft = foot;
  foot = foot->prev;
  delete ft;

  if (foot) foot->next = 0;
  else head = 0;

  return ret;
}

template <typename Tp>
Tp &Stack<Tp>::peak(void) {
  // 默认链表非空，为空直接抛异常
  if (!foot)
    throw std::out_of_range("there isn't any elements here");
  return *(foot->elem);
}

template <typename Tp>
bool Stack<Tp>::is_empty(void) {
  return head == 0;
}

template <typename Tp>
Stack<Tp> &Stack<Tp>::clear(void) {
  while (foot) {
    delete foot->elem;
    Node *ft = foot;
    foot = foot->prev;
    delete ft;
  }
  head = 0;
}


class Calculator {
 public:
  Calculator() {}
  double calc(const std::string expr);

 protected:
  double handler_point(double left, double right);
  double handler_pow(double left, double right);
  double handler_e(double left, double right);
  double handler_mul(double left, double right);
  double handler_div(double left, double right);
  double handler_mod(double left, double right);
  double handler_add(double left, double right);
  double handler_sub(double left, double right);

  enum Op { op_lt, op_gt,
            op_point,
            op_pow, op_e,
            op_mul, op_div, op_mod,
            op_add, op_sub };

  typedef double (Calculator::*PtrFunc) (double, double);  // 指向本类的成员函数的指针
  typedef Stack<Op> StkOp;
  typedef Stack<double> StkData;
  StkOp stk_ops;
  StkData stk_ods;

  std::map<const std::string, Op> char2op = {
    {".", op_point},
    {"e", op_e},
    {"**", op_pow},
    {"*", op_mul},
    {"/", op_div},
    {"%", op_mod},
    {"+", op_add},
    {"-", op_sub},
  };
  #ifdef DEBUG
  std::map<Op, const std::string> op2char = {
    {op_point, "."},
    {op_e, "e"},
    {op_pow, "**"},
    {op_mul, "*"},
    {op_div, "/"},
    {op_mod, "%"},
    {op_add, "+"},
    {op_sub, "-"},
  };
  #endif
  std::map<Op, int> op2priority = {  // 优先级
    {op_point, 10},
    {op_e, 9},
    {op_pow, 8},
    {op_mul, 7},
    {op_div, 7},
    {op_mod, 7},
    {op_add, 6},
    {op_sub, 6},
  };
  std::map<Op, PtrFunc> op2func = {
    {op_point, handler_point},
    {op_pow, handler_pow},
    {op_e, handler_e},
    {op_mul, handler_mul},
    {op_div, handler_div},
    {op_mod, handler_mod},
    {op_add, handler_add},
    {op_sub, handler_sub},
  };
};

double Calculator::calc(const std::string expr) {
  typedef std::string::const_iterator iter;
  enum Status { stat_digit, stat_op };
  // 单独使用一个枚举类型是为了少出现一些魔数，代码更有语义一点

  stk_ods.clear();
  stk_ops.clear();

  int at = 0, change_stat_when = 0;
  // 用于定位出错点
  std::string buf;
  Status stat = stat_digit;
  // 指示上次处理的字符是数字还是非数字，以在下面将数字/运算符分开

  if (!expr.empty()) {
    while (expr[at] == ' ') ++at;
    if (!isdigit(expr[at]))
      throw SyntaxException(at, "The expression must start with digits");
  }

  at = 0;
  for (iter i = expr.cbegin(); i != expr.cend(); ++at, ++i) {
    if (*i == ' ') continue;

    if (isdigit(*i)) {
      if (stat == stat_digit) buf += *i;  // 累积数字，处理多于一位的数字
      else {
        // 如果是从非数字转换到数字，说明非数字(运算符)已经结束了，可以进行处理了
        if (char2op.find(buf) == char2op.end())
          throw SyntaxException(change_stat_when, "Unknown operator " + buf);

        while (!stk_ops.is_empty() && op2priority[char2op[buf]] < op2priority[stk_ops.peak()]) {
          // 如果栈顶的运算优先级比正在处理的高，那么要提前处理好栈中已有的优先级较高的运算符
          // 直到栈顶的运算符优先级相同或更低
          // (这种情况下，在稍后按照从右往左(栈的特性)的运算顺序则刚好符合优先级计算规则，所以不需要特殊处理)
          double right = stk_ods.pop(), left = stk_ods.pop();
          // 小坑：栈 LIFO，所以先 pop() 的是右操作数，后为左
          Op op = stk_ops.pop();
          stk_ods.push((this->*op2func[op])(left, right));
          // 大坑：指向类的成员函数的指针，要调用则必须(显式)提供类的实例对象
          // 在这里体现为必须 (this -> *xxx)(xxx) 而不能 xxx(xxx)
          PRINT_PROCEDURE_IF_DEBUG;
        }

        stk_ops.push(char2op[buf]);
        // 处理完留在 buf 中之前的运算符了，清空并把现在在处理的数字整进 buf
        stat = stat_digit;
        buf.clear();
        buf += *i;

        change_stat_when = at;
      }

    } else {
      // 类似上文
      if (stat == stat_op) buf += *i;
      else {
        stk_ods.push(std::stoi(buf));
        stat = stat_op;
        buf.clear();
        buf += *i;

        change_stat_when = at;
      }
    }
  }

  if (!buf.empty()) {
    if (stat == stat_op)
      throw SyntaxException(at, "Unexpected end-of-expression");
    stk_ods.push(std::stoi(buf));  // 最后的数字入栈一下
  } else {
    return 0.0 / 0.0;  // Na
  }

  while (!stk_ops.is_empty()) {  // 现在优先级已经没问题了，直接从右往左计算即可
    double right = stk_ods.pop(), left = stk_ods.pop();
    Op op = stk_ops.pop();
    stk_ods.push((this->*op2func[op])(left, right));
    PRINT_PROCEDURE_IF_DEBUG;
  }

  return stk_ods.peak();  // 栈顶的数字即为计算结果
}


double Calculator::handler_point(double left, double right) {
  // 小数通过优先级最高的“点号”运算符来处理
  std::stringstream ss;
  ss << (int) left << '.' << (int) right;
  return std::stod(ss.str());
}

double Calculator::handler_pow(double left, double right) {
  return pow(left, right);
}

double Calculator::handler_e(double left, double right) {
  return left * pow(10, (int) right);
}

double Calculator::handler_mul(double left, double right) {
  return left * right;
}

double Calculator::handler_div(double left, double right) {
  return left / right;
}

double Calculator::handler_mod(double left, double right) {
  return ((int) left) % ((int) right);
}

double Calculator::handler_add(double left, double right) {
  return left + right;
}

double Calculator::handler_sub(double left, double right) {
  return left - right;
}


int main() {
  using namespace std;
  Calculator calc;
  string buf;
  
  cout << ">>> ";
  while (getline(cin, buf)) {
    double ret;
    try {
      ret = calc.calc(buf);
      cout << "==> " << ret << endl;
    } catch (SyntaxException e) {
      e.show();
    }
    cout << ">>> ";
  }

  cout << "Bye ;)" << endl;
}
