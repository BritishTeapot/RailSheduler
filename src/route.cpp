#include "route.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>

Route::Route(std::vector<int64_t> path) { this->path = path; }

int64_t Route::getPosition(int index) { return path.at(index); }

int Route::getLength() { return path.size(); }

Route Route::fromFile(std::ifstream &file) {
  int lenght;
  std::vector<int64_t> path;

  file >> lenght;

  for (int i = 0; i < lenght; i++) {
    int element;
    file >> element;
    /*
     * Following code checks for subpaths like "a -> a" (a.k.a. duplicate
     * consecutive veritcies), which do not make sense in the job-shop model.
     * Techiaclly speaking they do not pose any significant problem,
     * however if we assume no consecutive duplicates are present,
     * code can be simplified a lot.
     */
    if ((i != 0) && (element == path.back())) {
      std::cout << "Bad file format: consecutive duplicate path veritcies."
                << std::endl;
      exit(EXIT_FAILURE);
    }
    path.push_back(element);
  }

  return Route(path);
}
