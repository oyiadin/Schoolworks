#include <iostream>
#include <string>
#include "../predefine.h"
#include "list.h"

using namespace std;

bool visit(string &str) {
	cout << "visit: " << str << endl;
	return true;
}


int main() {
    _List<string> list;
    cout << "the list " << (list.is_empty() ? "is" : "isn't")
         << " empty now" << endl;
    cout << "now insert something" << endl;
    list.insert(1, "b");
    list.insert(1, "a");
    cout << "the list " << (list.is_empty() ? "is" : "isn't")
         << " empty now" << endl;
    cout << "location of a, b: "
         << list.locate_elem("a") << ", "
         << list.locate_elem("b") << endl;
    cout << "the element prior to 'b' is '" << list.prior_elem("b") << "'" << endl;
    cout << "the next element of 'a' is '" << list.next_elem("a") << "'" << endl;
    list.traverse(visit);
    cout << "now delete something" << endl;
    list.del(1);
    cout << "the element at location 1 is: " << list.get_elem(1) << endl;
    cout << "now clear all the list" << endl;
    list.clear();
    cout << "the list " << (list.is_empty() ? "is" : "isn't")
         << " empty now" << endl;
}
