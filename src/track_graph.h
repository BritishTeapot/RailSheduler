
#ifndef TRACK_GRAPH_H
#define TRACK_GRAPH_H

#include "route.h"
#include <cstdint>
#include <fstream>
#include <map>
#include <utility>
#include <vector>

class TrackGraph {
private:
  std::map<track_t, std::vector<track_t>> adjacencyMap;
  std::map<track_t, std::vector<track_t>> conflictMap;

public:
  TrackGraph();
  TrackGraph(std::map<track_t, std::vector<track_t>> adjacencyMap,
             std::map<track_t, std::vector<track_t>> conflictMap);
  static TrackGraph fromFile(std::ifstream &file);
  bool isRouteValid(Route &route);
  std::vector<track_t> getTracks();
  std::vector<track_t> getAdjacent(track_t track);
};

#endif
