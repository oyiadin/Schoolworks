#include <iostream>
#include <string>

bool do_add(int &low, int &high, const int max, const int summand=0);
bool do_sub(int &low, int &high, const int max, const int minuend=0);

class Time {
 public:
  Time(const int s=0, const int m=0, const int h=0, const int d=0)
      : second(s), minute(m), hour(h), day(d) {}
  friend inline std::istream &operator>>(std::istream &is, Time &t);
  friend inline std::ostream &operator<<(std::ostream &os, Time &t);
  inline Time operator+(Time &);
  inline Time operator-(Time &);
  void setTime(const int s=0, const int m=0, const int h=0, const int d=0);
  int getAttribute(const std::string &attr);
  int getAttribute(const char *attr);
  Time &add(const Time &);
  Time &add(const int sec);
  Time &sub(const Time &);
  Time &sub(const int sec);
 private:
  int day, hour, minute, second;
};

inline std::istream &operator>>(std::istream &is, Time &t) {
  is >> t.day >> t.hour >> t.minute >> t.second;
  return is;
}

inline std::ostream &operator<<(std::ostream &os, Time &t) {
  os << t.day << ' ' << t.hour << ':' << t.minute << ':' << t.second;
  return os;
}

inline Time Time::operator+(Time &t) {
  Time ret(*this);
  return ret.add(t);
}

inline Time Time::operator-(Time &t) {
  Time ret(*this);
  return ret.sub(t);
}

void Time::setTime(const int s, const int m, const int h, const int d) {
  second = s;
  minute = m;
  hour = h;
  day = d;
}

int Time::getAttribute(const std::string &attr) {
  if (attr == "second") return second;
  else if (attr == "minute") return minute;
  else if (attr == "hour") return hour;
  else if (attr == "day") return day;
}

int Time::getAttribute(const char *attr) {
  const std::string s(attr);
  return getAttribute(s);
}

Time &Time::add(const Time &summand) {
  do_add(second, minute, 60, summand.second);
  do_add(minute, hour, 60, summand.minute);
  do_add(hour, day, 24, summand.hour);
  day += summand.day;
  return *this;
}

Time &Time::add(const int sec) {
  do_add(second, minute, 60, sec)
    && do_add(minute, hour, 60)
    && do_add(hour, day, 24);
  return *this;
}

Time &Time::sub(const Time &minuend) {
  do_sub(second, minute, 60, minuend.second);
  do_sub(minute, hour, 60, minuend.minute);
  do_sub(hour, day, 24, minuend.hour);
  day -= minuend.day;
  return *this;
}

Time &Time::sub(const int sec) {
  do_sub(second, minute, 60, sec)
    && do_sub(minute, hour, 60)
    && do_sub(hour, day, 24);
  return *this;
}


bool do_add(int &low, int &high, const int max, const int summand) {
  // returns `carry flag`
  low += summand;
  if (low >= max) {
    high += low / max;
    low %= max;
    return true;
  }
  return false;
}

bool do_sub(int &low, int &high, const int max, const int minuend) {
  low -= minuend;
  if (low < 0) {
    high -= ((-low - 1) / max) + 1;
    low = (low % max + max) % max;
    return true;
  }
  return false;
}


int main() {
  using namespace std;
  Time time1, time2;
  while (true) {
    int d, h, m, s;
    cout << ">>> ";
    if (!(cin >> time1)) break;
    cout << ">>> ";
    if (!(cin >> time2)) break;

    Time temp(time1 + time2);
    cout << "==> " << temp << endl;
    temp = time1 - time2;
    cout << "==> " << temp << endl;

    cout << "==> " << temp.getAttribute("second") << endl;
    temp.add(1);
    cout << "==> " << temp.getAttribute("second") << endl;
  }
  cout << "goodbye ;)" << endl;
}
