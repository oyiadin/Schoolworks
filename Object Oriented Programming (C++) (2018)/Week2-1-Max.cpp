// design a `max` function
#include <iostream>
using namespace std;

#define SIZE 100

double &max(double array[], int total) {
  // assert total > 0
  int max_i = 0;
  for (int i = 1; i < total; ++i)
    if (array[i] >= array[max_i])
      max_i = i;

  return array[max_i];
}

int main() {
  int total = 0;
  double array[SIZE];

  cout << "press Ctrl+Z then Enter to get the maximum one:" << endl;
  while (cin >> array[total++]);
  // total++ will also be executed when Ctrl+Z is pressed
  cout << "the maximum one is " << max(array, --total) << endl;

  cout << "then I will do some magic with the help of reference XD:" << endl;
  max(array, total) = 233333;
  for (int i = 0; i < total; ++i)
    cout << array[i] << " ";
  cout << endl;
}
