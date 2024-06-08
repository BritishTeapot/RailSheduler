#ifndef ROUTE_H
#define ROUTE_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <utility>
#include <vector>

using track_t = int64_t;

struct routeVertex {
  track_t track;

  // here min_time is minimum time the
  // train needs to spend on this track to
  // leave it in seconds
  uint32_t min_time;
  // the time has to be an integer because CP-SAT
  // only works with integers
};

class Route {
private:
  std::vector<routeVertex> path;

  // it is a sum of all min_time values
  uint32_t route_time;
  // it is useful to compute this at construction
  // time, because routes might be reused

public:
  Route();
  Route(std::vector<routeVertex> path);
  Route(std::vector<std::pair<track_t, uint32_t>> path);
  track_t getPosition(int index);
  routeVertex getVertex(int index);
  size_t getLength();
  uint32_t getTime();
  static Route fromFile(std::ifstream &file);
};

#endif
