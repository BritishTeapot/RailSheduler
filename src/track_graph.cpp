#include "track_graph.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#define range(i, n)                                                            \
  uint32_t i = 0;                                                              \
  i < n;                                                                       \
  i++

TrackGraph::TrackGraph(std::map<track_t, std::vector<track_t>> adjacencyMap,
                       std::map<track_t, std::vector<track_t>> conflictMap) {
  this->conflictMap = conflictMap;
  this->adjacencyMap = adjacencyMap;
}

TrackGraph::TrackGraph() {} // default constructor for an empty graph

TrackGraph TrackGraph::fromFile(std::ifstream &file) {
  uint32_t n;
  std::map<track_t, std::vector<track_t>> adjacencyMap;
  std::map<track_t, std::vector<track_t>> conflictMap;
  file >> n;
  for (range(i, n)) {
    std::vector<track_t> adjacent;
    std::vector<track_t> conflicting;
    track_t track;
    uint32_t nadjacent, nconflicting;
    // track - track number, nadjacent - number of adjacent verticies,
    // nconflicting - number of conflicting verticies

    if (!file.is_open()) {
      std::cout << "Bad ifstream in inputTrack function." << std::endl;
    }

    file >> track;
    file >> nadjacent;

    for (uint32_t j = 0; j < nadjacent; j++) {
      track_t element;
      file >> element;
      adjacent.push_back(element);
    }

    file >> nconflicting;

    for (uint32_t j = 0; j < nconflicting; j++) {
      track_t element;
      file >> element;
      conflicting.push_back(element);
    }

    adjacencyMap[track] = adjacent;
    conflictMap[track] = conflicting;
  }

  return TrackGraph(adjacencyMap, conflictMap);
}

bool TrackGraph::isRouteValid(Route &route) {
  uint32_t length = route.getLength();
  for (uint32_t i = 0; i < length - 1; i++) {
    if (i > 0) {
      auto adjacent = adjacencyMap.find(route.getPosition(i));

      // here we presume the route can't contain path "a -> a"
      // because that does not make sense in the job-shop graph
      bool adjacent_to_next =
          (adjacent != adjacencyMap.end()) &&
          (std::find(adjacent->second.begin(), adjacent->second.end(),
                     route.getPosition(i + 1)) != adjacent->second.end());

      if (!adjacent_to_next)
        return false;
    }
  }
  return true;
}

std::vector<track_t> TrackGraph::getTracks() {
  std::vector<track_t> tracks;
  for (auto tup : adjacencyMap) {
    tracks.push_back(tup.first);
  }

  return tracks;
}

std::vector<track_t> TrackGraph::getAdjacent(track_t track) {
  return adjacencyMap[track];
}
