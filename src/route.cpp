#include "route.h"
#include <vector>

Route::Route(std::vector<int> path) { this->path = path; }

int Route::getPosition(int index) { return path.at(index); }

Route Route::fromFile(std::ifstream &file) {
  int lenght;
  std::vector<int> path;

  file >> lenght;

  for (int i = 0; i < lenght; i++) {
    int element;
    file >> element;
    path.push_back(element);
  }

  return Route(path);
}
