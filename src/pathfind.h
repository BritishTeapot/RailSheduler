
#ifndef PATH_FIND_H
#define PATH_FIND_H

#include "route.h"
#include "track_graph.h"
#include <cstdint>
#include <queue>
#include <vector>

/* this is an algo to find all possible paths between
 * two places in a graph
 * basically is a simple BFS, nothing fancy.
 */
std::vector<Route> findAllRoutes(TrackGraph &graph, track_t from, track_t to);

#endif // !PATH_FIND_H
