#include <iostream>
#include <cmath>

class Polar;
class Rectangular;

class Polar {
 public:
  friend class Rectangular;
  // To be able to access the private member X and Y
  Polar(double r = 0, double t = 0) : radius(r), theta(t) {}
  Polar(const Polar &p) : radius(p.radius), theta(p.theta) {}
  // In fact, I don't think that this copy constructor is necessary
  Polar(const Rectangular &r);
  friend inline std::ostream &operator<<(std::ostream &o, const Polar &p);
  friend std::istream &operator>>(std::istream &i, Polar &p);

 private:
  static const double PI;
  double radius = 0, theta = 0;
};

const double Polar::PI = acos(-1.0);

class Rectangular {
 public:
  friend class Polar;
  Rectangular(double x = 0, double y = 0) : X(x), Y(y) {}
  Rectangular(const Rectangular &r) : X(r.X), Y(r.Y) {}
  Rectangular(const Polar &p);
  friend inline std::ostream &operator<<(std::ostream &o, const Rectangular &r);
  friend inline std::istream &operator>>(std::istream &i, Rectangular &r);

 private:
  double X, Y;
};


Polar::Polar(const Rectangular &r) {
  radius = sqrt(pow(r.X, 2) + pow(r.Y, 2));
  theta = r.X ? atan(r.Y / r.X) : 0;
  // if r.X < 0 then fix the theta
  if (r.X < 0) theta += PI / 2;
  // let it be in the range of [0, 2*PI)
  if (theta < 0) theta += 2 * PI;
}

inline std::ostream &operator<<(std::ostream &o, const Polar &p) {
  o << "Polar(" << p.radius << ", " << p.theta << ")";
  return o;
}

std::istream &operator>>(std::istream &i, Polar &p) {
  i >> p.radius >> p.theta;
  // assure that radius is positive
  if (p.radius < 0) {
    p.radius = -p.radius;
    p.theta += Polar::PI;
  }
  // assure that theta is within [0, 2*PI)
  while (p.theta < 0) p.theta += 2 * Polar::PI;
  while (p.theta > 2 * Polar::PI) p.theta -= 2 * Polar::PI;
  return i;
}


Rectangular::Rectangular(const Polar &p) {
  X = p.radius * cos(p.theta);
  Y = p.radius * sin(p.theta);
}

inline std::ostream &operator<<(std::ostream &o, const Rectangular &r) {
  o << "Rectangular(" << r.X << ", " << r.Y << ")";
  return o;
}

inline std::istream &operator>>(std::istream &i, Rectangular &r) {
  i >> r.X >> r.Y;
  return i;
}


int main() {
  using namespace std;
  Polar p;
  Rectangular r;
  while (1) {
    cout << ">>> input X and Y: ";
    cin >> r;
    cout << "==> " << r << endl;
    p = r;
    cout << "==> " << p << endl << endl;

    cout << ">>> input radius and theta: ";
    cin >> p;
    cout << "==> " << p << endl;
    r = p;
    cout << "==> " << r << endl << endl;
  }
}
