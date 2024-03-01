#include "track_graph.h"
#include <iostream>

#define range(i, n)                                                            \
  int i = 0;                                                                   \
  i < n;                                                                       \
  i++

TrackGraph::TrackGraph(std::map<int, std::set<int>> adjacencyMap,
                       std::map<int, std::set<int>> conflictMap) {
  this->conflictMap = conflictMap;
  this->adjacencyMap = adjacencyMap;
}

TrackGraph TrackGraph::fromFile(std::ifstream &file) {
  int n;
  std::map<int, std::set<int>> adjacencyMap;
  std::map<int, std::set<int>> conflictMap;
  file >> n;
  for (range(i, n)) {
    std::set<int> adjacent;
    std::set<int> conflicting;
    int track, nadjacent, nconflicting;
    // track - track number, nadjacent - number of adjacent verticies,
    // nconflicting - number of conflicting verticies

    if (file.is_open()) {
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
  for (int i = 0; i < lenght; i++) {
    if (i > 0) {
      auto adjacent = adjacencyMap.find(route.getPosition(i - 1));

      bool is_finished = (route.getPosition(i) == -2);
      bool previous_vertex_has_adjacent = (adjacent != adjacencyMap.end());
      if (!is_finished) {
        if (!previous_vertex_has_adjacent) {
          return false;
        } else {
          bool is_previous_vertex_connected =
              adjacent->second.find(route.getPosition(i)) !=
              adjacent->second.end();
          bool is_moving = adjacent->first != route.getPosition(i);

          if (!is_previous_vertex_connected && is_moving) {
            return false;
          }
        }
      }
    }
  }
  return true;
}
