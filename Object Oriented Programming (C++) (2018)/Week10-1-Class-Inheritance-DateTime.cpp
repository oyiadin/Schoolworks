#include <iostream>
#include <iomanip>

class Date;
class Time;
class DateTime;
inline std::ostream &operator<<(std::ostream &os, const Date &rhs);
inline std::istream &operator>>(std::istream &is, Date &rhs);
inline std::ostream &operator<<(std::ostream &os, const Time &rhs);
inline std::istream &operator>>(std::istream &is, Time &rhs);
inline std::ostream &operator<<(std::ostream &os, const DateTime &rhs);
inline std::istream &operator>>(std::istream &is, DateTime &rhs);

void do_sub(int &low, int &high, const int max, const int minuend=0);


class Date {
 public:
  Date(int y = 0, int m = 0, int d = 0)
      : year(y), month(m), day(d) {}
  Date &sub(const Date &rhs);
  Date operator-(const Date &rhs);
  friend std::ostream &operator<<(std::ostream &os, const Date &rhs);
  friend std::istream &operator>>(std::istream &is, Date &rhs);
 protected:
  int year, month, day;
};

std::ostream &operator<<(std::ostream &os, const Date &rhs) {
  return os << std::setw(4) << std::setfill('0') << rhs.year << '/'
            << std::setw(2) << std::setfill('0') << rhs.month << '/'
            << std::setw(2) << std::setfill('0') << rhs.day;
}

std::istream &operator>>(std::istream &is, Date &rhs) {
  return is >> rhs.year >> rhs.month >> rhs.day;
}

Date &Date::sub(const Date &rhs) {
  do_sub(day, month, 30, rhs.day);
  do_sub(month, year, 12, rhs.year);
  year -= rhs.year;
  return *this;
}

Date Date::operator-(const Date &rhs) {
  Date temp = *this;
  return temp.sub(rhs);
}


class Time {
 public:
  Time(int h = 0, int m = 0, int s = 0)
      : hour(h), minute(m), second(s) {}
  Time &sub(const Time &rhs);
  Time operator-(const Time &rhs);
  friend std::ostream &operator<<(std::ostream &os, const Time &rhs);
  friend std::istream &operator>>(std::istream &is, Time &rhs);
 protected:
  int hour, minute, second;
};

std::ostream &operator<<(std::ostream &os, const Time &rhs) {
  return os << std::setw(2) << std::setfill('0') << rhs.hour << ':'
            << std::setw(2) << std::setfill('0') << rhs.minute << ':'
            << std::setw(2) << std::setfill('0') << rhs.second;
}

std::istream &operator>>(std::istream &is, Time &rhs) {
  return is >> rhs.hour >> rhs.minute >> rhs.second;
}

Time &Time::sub(const Time &rhs) {
  do_sub(second, minute, 60, rhs.second);
  do_sub(minute, hour, 60, rhs.minute);
  hour -= rhs.hour;
  return *this;
}

Time Time::operator-(const Time &rhs) {
  Time temp = *this;
  return temp.sub(rhs);
}


class DateTime : protected Date, protected Time {
 public:
  DateTime(int y = 0, int m = 0, int d = 0, int H = 0, int M = 0, int S = 0)
      : Date(y, m, d), Time(H, M, S) {}
  DateTime &sub(const DateTime &rhs);
  DateTime operator-(const DateTime& rhs);
  friend std::ostream &operator<<(std::ostream &os, const DateTime &rhs);
  friend std::istream &operator>>(std::istream &is, DateTime &rhs);
};

std::ostream &operator<<(std::ostream &os, const DateTime &rhs) {
  return os << (Date &) rhs << ' ' << (Time &) rhs;
}

std::istream &operator>>(std::istream &is, DateTime &rhs) {
  return is >> (Date &) rhs >> (Time &) rhs;
}

DateTime &DateTime::sub(const DateTime &rhs) {
  ((Date *) this)->sub((Date &) rhs);
  ((Time *) this)->sub((Time &) rhs);
  // if (hour < 0)
  do_sub(hour, day, 60, 0);
  return *this;
}

DateTime DateTime::operator-(const DateTime &rhs) {
  DateTime temp = *this;
  return temp.sub(rhs);
}


void do_sub(int &low, int &high, const int max, const int minuend) {
  low -= minuend;
  if (low < 0) {
    high -= ((-low - 1) / max) + 1;
    low = (low % max + max) % max;
  }
}


int main() {
  using namespace std;
  DateTime dt1, dt2;
  while (cin >> dt1 >> dt2) {
    cout << "dt1: " << dt1 << endl;
    cout << "dt2: " << dt2 << endl;
    cout << "1-2: " << dt1 - dt2 << endl;
  }
  cout << "see u" << endl;
}
