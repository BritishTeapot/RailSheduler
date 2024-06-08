#include "track_graph.h"
#include "route.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <queue>
#include <set>
#include <vector>

#define TIME_SECTION 5

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

std::vector<track_t> TrackGraph::getConflicting(track_t track) {
  return conflictMap[track];
}

std::vector<Route> TrackGraph::findAllRoutes(track_t from, track_t to) {

  std::queue<
      std::pair<std::vector<std::pair<track_t, uint32_t>>, std::set<track_t>>>
      q;
  std::vector<Route> routes;
  q.push({{{from, TIME_SECTION}}, std::set<track_t>()});
  while (!q.empty()) {
    auto [next, visited] = q.front();
    q.pop();

    if (next.back().first == to) {
      routes.push_back(Route(next));
    } else if (visited.find(next.back().first) != visited.end()) {
      continue;
    } else {
      visited.insert(next.back().first);
      for (auto t : getAdjacent(next.back().first)) {
        next.push_back({t, TIME_SECTION});
        q.push({next, visited});
        next.pop_back();
      }
    }
  }

  return routes;
}
