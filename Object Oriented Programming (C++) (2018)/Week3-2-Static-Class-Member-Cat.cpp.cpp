#include <iostream>

class Cat {
 public:
  Cat() {
    numOfCats++;
    std::cout << "Another cat was born!" << std::endl;
  }
  ~Cat() {
    numOfCats--;
    std::cout << "A cat left QAQ" << std::endl;
  }
  static int getNumOfCats() { return numOfCats; }
 private:
  static int numOfCats;
};

int Cat::numOfCats = 0;

void f() {
  using namespace std;
  Cat cat1;
  cout << "We have " << Cat::getNumOfCats() << " cats now" << endl;
}

int main() {
  using namespace std;
  cout << "We have " << Cat::getNumOfCats() << " cats now" << endl;
  f();
  Cat cat1, cat2;
  cout << "We have " << Cat::getNumOfCats() << " cats now" << endl;
}
