
#ifndef TRACK_GRAPH_H
#define TRACK_GRAPH_H

#include "route.h"
#include <cstdint>
#include <fstream>
#include <map>
#include <set>
#include <utility>
class TrackGraph {
private:
  std::map<int64_t, std::set<int64_t>> adjacencyMap;
  std::map<int64_t, std::set<int64_t>> conflictMap;

public:
  TrackGraph();
  TrackGraph(std::map<int64_t, std::set<int64_t>> adjacencyMap,
             std::map<int64_t, std::set<int64_t>> conflictMap);
  static TrackGraph fromFile(std::ifstream &file);
  bool isRouteValid(Route &route);
  std::set<int64_t> getTracks();
};

#endif
