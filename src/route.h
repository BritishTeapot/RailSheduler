#ifndef ROUTE_H
#define ROUTE_H

#include <cstdint>
#include <cstring>
#include <fstream>
#include <utility>
#include <vector>

struct routeVertex {
  int64_t track;

  // here min_time is minimum time the
  // train needs to spend on this track to
  // leave it in seconds
  int64_t min_time;
  // the time has to be an integer because CP-SAT
  // only works with integers
};

class Route {
private:
  std::vector<routeVertex> path;

public:
  Route();
  Route(std::vector<routeVertex> path);
  Route(std::vector<std::pair<int64_t, int64_t>> path);
  int64_t getPosition(int index);
  routeVertex getVertex(int index);
  int getLength();
  static Route fromFile(std::ifstream &file);
};

#endif
