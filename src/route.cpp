#include "route.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>

Route::Route(std::vector<std::pair<int64_t, int64_t>> path) {
  std::vector<routeVertex> new_path;
  for (auto i : path) {
    routeVertex rv;
    rv.track = i.first;
    rv.min_time = i.second;
    new_path.push_back(rv);
  }
  this->path = new_path;
}

Route::Route() {} // default constructor for an empty path

int64_t Route::getPosition(int index) { return path.at(index).track; }

int Route::getLength() { return path.size(); }

Route Route::fromFile(std::ifstream &file) {
  size_t lenght;
  std::vector<routeVertex> path;

  file >> lenght;

  for (size_t i = 0; i < lenght; i++) {
    int64_t track;
    int64_t time;
    file >> track;
    file >> track;
    /*
     * Following code checks for subpaths like "a -> a" (a.k.a. duplicate
     * consecutive veritcies), which do not make sense in the job-shop model.
     * Techiaclly speaking they do not pose any significant problem,
     * however if we assume no consecutive duplicates are present,
     * code can be simplified a lot.
     */
    if ((i != 0) && (track == path.back().track)) {
      std::cout << "Bad file format: consecutive duplicate path veritcies."
                << std::endl;
      exit(EXIT_FAILURE);
    }
    routeVertex rv;
    rv.track = track;
    rv.min_time = time;
    path.push_back(rv);
  }

  return Route(path);
}
