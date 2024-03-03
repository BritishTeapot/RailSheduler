
#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "route.h"
#include "track_graph.h"
#include <vector>
class Schedule {
private:
  std::vector<Route> routes;
  TrackGraph track_graph;

public:
  Schedule(std::vector<Route> routes, TrackGraph track_graph);
};

#endif
