
#include "pathfind.h"
#include "route.h"
#include "track_graph.h"
#include <cstdint>
#include <set>
#include <vector>

#define TIME_SECTION 5

std::vector<Route> findAllRoutes(TrackGraph &graph, track_t from, track_t to) {

  std::queue<
      std::pair<std::vector<std::pair<track_t, int64_t>>, std::set<track_t>>>
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
      for (auto t : graph.getAdjacent(next.back().first)) {
        next.push_back({t, TIME_SECTION});
        q.push({next, visited});
        next.pop_back();
      }
    }
  }

  return routes;
}
