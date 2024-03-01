
#ifndef TRACK_GRAPH_H
#define TRACK_GRAPH_H

#include "route.h"
#include <fstream>
#include <map>
#include <set>
#include <utility>
class TrackGraph {
private:
  std::map<int, std::set<int>> adjacencyMap;
  std::map<int, std::set<int>> conflictMap;

public:
  TrackGraph(std::map<int, std::set<int>> adjacencyMap,
             std::map<int, std::set<int>> conflictMap);
  static TrackGraph fromFile(std::ifstream &file);
  bool isRouteValid(Route &route);
  std::tuple<std::vector<Route>::iterator, std::vector<Route>::iterator,
             int // position of the conflicting verticies
             >
  findConflicting(std::vector<Route> routes);
};

#endif
