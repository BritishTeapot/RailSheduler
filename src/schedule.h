
#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "route.h"
#include "track_graph.h"
#include <vector>
class Schedule {
private:
  std::vector<Route> solution;
  std::vector<Route> routes;
  std::vector<std::vector<Route>> optroutes;
  TrackGraph track_graph;
  bool is_solved;

public:
  // optroutes are routes with alternative paths, e.g. vectors or routes
  // where exctly one must be chosen for the schedule.
  Schedule(std::vector<Route> routes, std::vector<std::vector<Route>> optroutes,
           TrackGraph track_graph);
  void solve();
  bool isSolved();
  // this must be the same as routes.size() + optroutes.size().
  int getRoutesCount();
  // if not solved, will return empty route
  Route getRoute(int index);
};

#endif
