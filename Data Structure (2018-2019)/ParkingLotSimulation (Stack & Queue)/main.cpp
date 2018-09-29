#include <iostream>
#include <cassert>
#include "stack.h"
#include "queue.h"
using namespace std;

#define PRICE 2
#define MAXSIZE 2

struct Car {
  int id;
  int arrival_t;
};


enum Where { IN_PARK, WAIT_OUTSIDE };
struct ArrivalResult {
  Where where;
  int pos;
};

struct DepartureResult {
  int time_during;
  int money;
};


Stack<Car> in_park(MAXSIZE), temp_out_park;
Queue<Car> wait_outside;


ArrivalResult deal_arrival(int id, int time) {
  Car temp = {id, time};
  ArrivalResult ret;

  if (!in_park.push(temp)) {
    wait_outside.push(temp);
    ret.where = WAIT_OUTSIDE;
    ret.pos = wait_outside.length;
    return ret;
  }

  ret.where = IN_PARK;
  ret.pos = in_park.length;
  return ret;
}

void print_arrival_result(const ArrivalResult &res) {
  switch (res.where) {
    case IN_PARK:
      cout << "Arv: in-park, at " << res.pos << endl;
      break;
    case WAIT_OUTSIDE:
      cout << "Arv: waiting, at " << res.pos << endl;
      break;
  }
}


DepartureResult deal_departure(int id, int time) {
  assert(temp_out_park.is_empty());
  DepartureResult ret;

  while (1) {
    Car temp = in_park.pop();
    if (temp.id != id) {
      temp_out_park.push(temp);
    } else {
      ret.time_during = time - temp.arrival_t;
      ret.money = PRICE * ret.time_during;
      break;
    }
  }

  while (!temp_out_park.is_empty())
    in_park.push(temp_out_park.pop());

  if (!wait_outside.is_empty()) {
    Car temp = wait_outside.pop();
    temp.arrival_t = time;
    in_park.push(temp);
  }

  return ret;
}

void print_departure_result(const DepartureResult &res) {
  cout << "Dpt: time=" << res.time_during << ", money=" << res.money << endl;
}


void print_everything() {
  cout << "==== Statistics of Car Parking ====" << endl;
  cout << "In park:" << endl;

  Stack<Car> temp_for_output;
  while (!in_park.is_empty())
    temp_for_output.push(in_park.pop());

  while (!temp_for_output.is_empty()) {
    Car temp = temp_for_output.pop();
    cout << "[-] car_id=" << temp.id << ", arrival at " << temp.arrival_t << endl;
  }

  cout << "Waiting outside:" << endl;
  while (!wait_outside.is_empty()) {
    Car temp = wait_outside.pop();
    cout << "[-] car_id=" << temp.id << ", wait since " << temp.arrival_t << endl;
  }

  cout << "===================================" << endl;
}


int main() {
  char event;
  int id, time;
  while (cin >> event >> id >> time) {
    switch (event) {
      case 'A':
        print_arrival_result(deal_arrival(id, time));
        break;
      case 'D':
        print_departure_result(deal_departure(id, time));
        break;
      case 'E':
        print_everything();
        cout << "goodbye!" << endl;
        return 0;
      case 'P':
        print_everything();
        break;
      default:
        cout << "unknown option" << endl;
    }
  }

}