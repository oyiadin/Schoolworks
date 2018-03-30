#include <cmath>
#include <algorithm>
#include <iostream>

class Fraction;
// a `friend` statement isn't a declaration, so here is a required one.
inline std::istream &operator>>(std::istream &is, Fraction &frac);
inline std::ostream &operator<<(std::ostream &os, const Fraction &frac);

class Fraction {
 public:
  Fraction() : numerator(0), denominator(1) {}
  Fraction(const int n, const int d)
      : numerator(n), denominator(d) {
    standardize();
  }
  Fraction(const Fraction &frac)
      : numerator(frac.numerator), denominator(frac.denominator) {}
  ~Fraction() {}

  inline Fraction operator+() const;
  inline Fraction operator-() const;
  Fraction operator+(const Fraction &frac) const;
  Fraction operator-(const Fraction &frac) const;
  Fraction operator*(const Fraction &frac) const;
  Fraction operator/(const Fraction &frac) const;
  Fraction &operator++();  // ++a, returns a lvalue
  Fraction &operator--();
  Fraction operator++(int);  // a++, returns a rvalue
  Fraction operator--(int);

  bool operator==(const Fraction &frac) const;
  bool operator!=(const Fraction &frac) const;
  bool operator>(const Fraction &frac) const;
  bool operator<(const Fraction &frac) const;
  bool operator>=(const Fraction &frac) const;
  bool operator<=(const Fraction &frac) const;
  

  Fraction(const double decimal);
  Fraction(const int decimal);
  explicit operator double() const;
  explicit operator int() const;

  int operator[](unsigned int which);
  friend inline std::istream &operator>>(std::istream &is, Fraction &frac);
  friend inline std::ostream &operator<<(std::ostream &os, const Fraction &frac);

 private:
  int numerator, denominator;
  Fraction &standardize();
};

Fraction Fraction::operator+() const {
  return *this;
  // returns a new duplicate
}

Fraction Fraction::operator-() const {
  Fraction temp(*this);
  temp.numerator = -temp.numerator;
  return temp.standardize();
}

Fraction Fraction::operator+(const Fraction &frac) const {
  Fraction temp(*this);
  temp.numerator = numerator * frac.denominator +
                   frac.numerator * denominator;
  temp.denominator *= frac.denominator;
  return temp.standardize();
}

Fraction Fraction::operator-(const Fraction &frac) const {
  return operator+(-frac);
}

Fraction Fraction::operator*(const Fraction &frac) const {
  Fraction temp(frac);
  temp.numerator *= numerator;
  temp.denominator *= denominator;
  return temp.standardize();;
}

Fraction Fraction::operator/(const Fraction &frac) const {
  if (!frac.numerator)
    throw std::overflow_error("divided by zero");

  Fraction temp(*this);
  temp.numerator *= frac.denominator;
  temp.denominator *= frac.numerator;
  return temp.standardize();;
}

Fraction &Fraction::operator++() {
  numerator += denominator;  // let it + 1
  return standardize();;
}

Fraction &Fraction::operator--() {
  numerator -= denominator;  // let it - 1
  return standardize();;
}

Fraction Fraction::operator++(int) {
  Fraction temp(*this);
  numerator += denominator;
  return temp.standardize();;
}

Fraction Fraction::operator--(int) {
  Fraction temp(*this);
  numerator -= denominator;
  return temp.standardize();;
}

bool Fraction::operator==(const Fraction &frac) const {
  if (numerator == frac.numerator && 
      denominator == frac.denominator) {
    return true;
  } else {
    return false;
  }
}

bool Fraction::operator!=(const Fraction &frac) const {
  return !(*this == frac);
}

bool Fraction::operator>(const Fraction &frac) const {
  if ((numerator * frac.denominator) > 
      (frac.numerator * denominator)) {
    return true;
  } else {
    return false;
  }
}

bool Fraction::operator<(const Fraction &frac) const {
  return (*this == frac) ? false : !(*this > frac);
}

bool Fraction::operator>=(const Fraction &frac) const {
  return operator>(frac) || operator==(frac);
}

bool Fraction::operator<=(const Fraction &frac) const {
  return operator<(frac) || operator==(frac);
}

Fraction::Fraction(const double decimal) {
  numerator = (int) (decimal * 100);
  denominator = 100;
  standardize();
}

Fraction::Fraction(const int decimal) {
  numerator = decimal;
  denominator = 1;
  standardize();
}

Fraction::operator double() const {
  return (double) numerator / denominator;
}

Fraction::operator int() const {
  return numerator / denominator;
}

int Fraction::operator[](unsigned int which) {
  // By convention, this function should return a lvalue (reference)
  // But I don't think that a reference to a private member can be
  // (directly) exposed to an external scope
  // So, I decided to return a (int) instead of (int &)
  if (which == 0) {
    return numerator;
  } else if (which == 1) {
    return denominator;
  }
  throw std::invalid_argument("argument should be `0` or `1`");
}

std::istream &operator>>(std::istream &is, Fraction &frac) {
  Fraction temp(frac);
  is >> temp.numerator >> temp.denominator;
  if (is) {
    // if succeeds, store the new state
    // if fails, do not change the state of @frac
    frac = temp.standardize();
  }
  return is;
}

std::ostream &operator<<(std::ostream &os, const Fraction &frac) {
  os << "Fraction(" << frac.numerator << ", " << frac.denominator << ")";
  return os;
}

Fraction &Fraction::standardize() {
  if (numerator == 0) {
    denominator = 1;
    // setting @denominator = 1 is very useful
    // when we do some + - * / and so forth.

  } else if (denominator == 0) {
    throw std::overflow_error("divided by zero");

  } else {
    bool sign = (numerator > 0) ^ (denominator > 0);
    // @sign == 0 if @numerator and @denominator have the same sign
    // otherwise @sign == 1
    if (sign)
      numerator = -abs(numerator);
    else
      numerator = abs(numerator);
    denominator = abs(denominator);
    // if this fraction is negative,
    // make the @numerator negative while @denominator positive

    int n = abs(numerator), d = denominator;
    while (n && d) {
      if (n < d) std::swap(n, d);
      n -= d;
    }
    // now that @d is exactly the GCD of @numerator and @denominator,
    // we can just simplify this fraction
    numerator /= d;
    denominator /= d;
  }

  return *this;
  // make `return` easier because we can just write:
  //     return something.standardize();
  // other than:
  //     something.standardize();
  //     return something;
}

#define out(x) \
  std::cout << "==> " #x " = " << (x) << std::endl;
  // #x means wrapping x with the symbol "
  // Example:
  //     out(ahh)
  // will be expanded to be:
  //     std::cout << "==> " "ahh" " = " << (ahh) << std::endl;
  // Note: the compiler would automatically merge these (char [])
  // i.e. "a" "b" would just be treated like "ab"

int main() {
  using namespace std;
  Fraction frac1(2, 4);
  cout << "       testing constructor: " << frac1 << endl;
  Fraction frac2(frac1);
  cout << "  testing copy constructor: " << frac2 << endl;
  frac1 = 2;
  cout << "   testing int to Fraction: " << frac1 << endl;
  cout << "   testing Fraction to int: " << (int) frac1 << endl;
  frac1 = 1.25;
  cout << "testing double to Fraction: " << frac1 << endl;
  cout << "testing Fraction to double: " << (double) frac1 << endl;

  while (cout << "==========================================" << endl) {
    cout << ">>> frac1 = ";
    cin >> frac1;
    cout << ">>> frac2 = ";
    cin >> frac2;
    out(frac1);
    out(frac2);
    out(+frac1);
    out(-frac1);
    cout << endl;
    out(frac1 + frac2);
    out(frac1 - frac2);
    out(frac1 * frac2);
    out(frac1 / frac2);
    cout << endl;
    out(frac1++);
    out(++frac1);
    out(frac1--);
    out(--frac1);
    cout << endl;
    out(frac1 == frac2);
    out(frac1 != frac2);
    out(frac1 > frac2);
    out(frac1 < frac2);
    out(frac1 >= frac2);
    out(frac1 <= frac2);
    cout << endl;
    out(frac1[0]);
    out(frac1[1]);
    cout << endl;
    out(frac1 * 2);
    out(frac1 / 0.5);
    out(frac1 * 1.5);
    out(frac1 + 1.5);
  }

  return 0;
}
