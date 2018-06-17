#include <iostream>

// helloworld:
// ++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.

namespace bf {

class Parser;
std::istream &operator>>(std::istream &is, Parser &rhs);

class Parser {
 public:
  Parser() = default;
  ~Parser() = default;
  friend std::istream &operator>>(std::istream &is, Parser &rhs);

 protected:
  char data[1000] = {0};
  int ptr = 0;
  std::string buf;
  void callback_i(void);
};


std::istream &operator>>(std::istream &is, Parser &rhs) {
  is >> rhs.buf;
  if (is)
    rhs.callback_i();

  return is;
}

void Parser::callback_i(void) {
  for (auto i = buf.cbegin(); i != buf.cend(); ++i) {
    // std::cout << '[' << *i << "] ";
    switch (*i) {
      case '>':
        ++ptr;
        break;
      case '<':
        --ptr;
        break;
      case '+':
        ++data[ptr];
        break;
      case '-':
        --data[ptr];
        break;
      case '.':
        putchar(data[ptr]);
        break;
      case ',':
        data[ptr] = getchar();
        break;
      case '[':
        if (!data[ptr]) {
          while (*i++ != ']') continue;
          --i;
        }
        break;
      case ']':
        if (data[ptr]) {
          while (*(i-1) != '[') --i;
          --i;
        }
        break;
      default:
        // std::cout << "[Unknown operator: " << *i << "]" << std::endl;
        break;
    }
  }

  buf.clear();
}

}


int main() {
  bf::Parser parser;
  while (1) {
    std::cin >> parser;
  }
}