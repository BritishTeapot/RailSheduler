#include "schedule.h"
#include "route.h"
#include "track_graph.h"
#include <vector>

Schedule::Schedule(std::vector<Route> routes, TrackGraph track_graph) {
  this->track_graph = track_graph;
  this->routes = routes;
}
