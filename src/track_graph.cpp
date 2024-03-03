#include "track_graph.h"
#include <cstdint>
#include <iostream>

#define range(i, n)                                                            \
  int i = 0;                                                                   \
  i < n;                                                                       \
  i++

TrackGraph::TrackGraph(std::map<int64_t, std::set<int64_t>> adjacencyMap,
                       std::map<int64_t, std::set<int64_t>> conflictMap) {
  this->conflictMap = conflictMap;
  this->adjacencyMap = adjacencyMap;
}

TrackGraph TrackGraph::fromFile(std::ifstream &file) {
  int n;
  std::map<int64_t, std::set<int64_t>> adjacencyMap;
  std::map<int64_t, std::set<int64_t>> conflictMap;
  file >> n;
  for (range(i, n)) {
    std::set<int64_t> adjacent;
    std::set<int64_t> conflicting;
    int track, nadjacent, nconflicting;
    // track - track number, nadjacent - number of adjacent verticies,
    // nconflicting - number of conflicting verticies

    if (!file.is_open()) {
      std::cout << "Bad ifstream in inputTrack function." << std::endl;
    }

    file >> track;
    file >> nadjacent;

    for (int j = 0; j < nadjacent; j++) {
      int element;
      file >> element;
      adjacent.insert(element);
    }

    file >> nconflicting;

    for (int j = 0; j < nconflicting; j++) {
      int element;
      file >> element;
      conflicting.insert(element);
    }

    adjacencyMap[track] = adjacent;
    conflictMap[track] = conflicting;
  }

  return TrackGraph(adjacencyMap, conflictMap);
}

bool TrackGraph::isRouteValid(Route &route) {
  int lenght = route.getLength();
  for (int i = 0; i < lenght - 1; i++) {
    if (i > 0) {
      auto adjacent = adjacencyMap.find(route.getPosition(i));

      // here we presume the route can't contain path "a -> a"
      // because that does not make sense in the job-shop graph
      bool adjacent_to_next =
          (adjacent != adjacencyMap.end()) &&
          (adjacent->second.find(route.getPosition(i + 1)) !=
           adjacent->second.end());

      if (!adjacent_to_next)
        return false;
    }
  }
  return true;
}
