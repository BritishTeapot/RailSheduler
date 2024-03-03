
#ifndef ROUTE_H
#define ROUTE_H

#include <cstdint>
#include <fstream>
#include <vector>

class Route {
private:
  std::vector<int64_t> path;

public:
  Route();
  Route(std::vector<int64_t> path);
  int64_t getPosition(int index);
  int getLength();
  static Route fromFile(std::ifstream &file);
};

#endif
