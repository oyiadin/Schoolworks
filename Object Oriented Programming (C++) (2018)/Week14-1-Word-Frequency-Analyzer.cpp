#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;
#define BYPASS_LENGTH 55


class Exception {
 public:
  Exception(const string &msg) : message(msg) {}
  const string &what(void) { return message; }

 private:
  const string message;
};


class Filter {
 public:
  bool is_okay(string &str) {
    bool flag = false;
    for (string::iterator it = str.begin(); it != str.end(); it++)
      if (isalpha(*it))
        flag = true;

    if (flag && find(bypass_list, bypass_list+BYPASS_LENGTH, str)
        == bypass_list+BYPASS_LENGTH)
      return true;
    return false;
  }

 private:
  static const string bypass_list[BYPASS_LENGTH];
};

const string Filter::bypass_list[BYPASS_LENGTH] = {
  "the", "a", "an", "of", "to", "in", "and", "but", "that", "am", "is", "are",
  "by", "as", "for", "this", "be", "can", "with", "or", "it", "not", "from",
  "was", "on", "at", "he", "these", "those", "one", "we", "has", "his", "have",
  "also", "some", "which", "all", "about", "its", "us", "other", "when", "my",
  "they", "i", "you", "said", "had", "him", "her", "she", "were", "out", "up",
};


class Standardizer {
 public:
  char standardize(string &str) {
    string buf;
    for (string::iterator it = str.begin(); it != str.end(); it++) {
      if (find(ignore_chars.begin(), ignore_chars.end(), *it)
          != ignore_chars.end()) {
        continue;
      } else {
        buf.push_back(tolower(*it));
      }
    }
    str = buf;
  }

 private:
  static const string ignore_chars;
};

const string Standardizer::ignore_chars = "!\"\'()*+,./[\\]^`:;<=>?{}|~";


class Converter {
 public:
  Converter(const string &fn) : filename(fn) {}
  vector<string> &file2vector(void);
  void vector2file(void);

 private:
  typedef pair<string, unsigned int> PAIR;
  static bool cmp(const PAIR &a, const PAIR &b) { return a.second > b.second; }

  const string filename;
  vector<string> vec;
  map<string, unsigned int> times;
  Filter filter;
  Standardizer standardizer;
};


vector<string> &Converter::file2vector(void) {
  ifstream file(filename.c_str());

  if (file.fail()) {
    throw Exception("File not found: " + filename);

  } else {
    string buf;
    while (file >> buf) {
      standardizer.standardize(buf);
      if (filter.is_okay(buf))
        vec.push_back(buf);
    }
  }
}


void Converter::vector2file(void) {
  if (vec.empty()) {
    throw Exception("There isn't any words in the provided file.");
  }

  for (vector<string>::iterator it = vec.begin(); it != vec.end(); it++) {
    times[*it]++;
  }

  ofstream file((filename + ".analyzed").c_str());

  vector<PAIR> sorted_vec;
  for (map<string, unsigned int>::iterator it = times.begin();
       it != times.end(); it++) {
    sorted_vec.push_back(make_pair(it->first, it->second));
  }

  sort(sorted_vec.begin(), sorted_vec.end(), cmp);

  for (vector<PAIR>::iterator it = sorted_vec.begin();
       it != sorted_vec.end(); it++) {
    file << setw(5) << right << it->second << ' ' << it->first << endl;
  }

  cout << "the word appeared most frequently is `"
       << sorted_vec[0].first << '`' << endl;
  cout << "the complete analyzed data was put in `"
       << filename + ".analyzed" << '`' << endl;
}


int main(int argc, char *argv[]) {
  string filename;

  try {
    if (argc == 2) {
      filename = argv[1];
    } else if (argc == 1) {
      cout << "Input the filename please: ";
      cin >> filename;
    } else {
      throw Exception("too many arguments");
    }

    Converter con(filename);

    con.file2vector();
    con.vector2file();

  } catch (Exception e) {
    cout << "Error: " << e.what() << endl;
  } catch (...) {
    cout << "I'm sorry but something unknown happened. :(" << endl;
  }
}