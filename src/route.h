
#ifndef ROUTE_H
#define ROUTE_H

#include <fstream>
#include <vector>

class Route {
private:
  std::vector<int> path;

public:
  Route(std::vector<int> path);
  int getPosition(int index);
  int getLength();
  static Route fromFile(std::ifstream &file);
};

#endif
