#include "route.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>

Route::Route(std::vector<std::pair<track_t, uint32_t>> path, uint32_t overlap) {
  std::vector<routeVertex> new_path;
  route_time = 0;
  for (auto i : path) {
    routeVertex rv;
    rv.track = i.first;
    rv.min_time = i.second;
    route_time += i.second;
    new_path.push_back(rv);
  }
  this->path = new_path;
  this->overlap = overlap;
}

Route::Route(std::vector<routeVertex> path, uint32_t overlap) {
  this->path = path;
  route_time = 0;
  for (auto rv : path) {
    route_time += rv.min_time;
  }
  this->overlap = overlap;
}

Route::Route() { route_time = 0; } // default constructor for an empty path

track_t Route::getPosition(int index) { return path.at(index).track; }
routeVertex Route::getVertex(int index) { return path.at(index); }

size_t Route::getLength() { return path.size(); }

uint32_t Route::getTime() { return route_time; }

uint32_t Route::getOverlap() { return overlap; }

Route Route::fromFile(std::ifstream &file) {
  size_t lenght;
  uint32_t overlap;
  std::vector<routeVertex> path;

  file >> lenght;
  file >> overlap;

  for (size_t i = 0; i < lenght; i++) {
    track_t track;
    uint32_t time;
    file >> track;
    file >> time;
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

  return Route(path, overlap);
}
