#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "route.h"
#include "schedule.h"
#include "track_graph.h"

#define range(i, n)                                                            \
  int i = 0;                                                                   \
  i < n;                                                                       \
  i++

std::ifstream safeOpen(std::string filename) {
  std::ifstream file;
  file.open(filename, std::ios::in);

  if (file.is_open()) {
    return file;
  } else {
    std::string msg = "Failed to open file ";
    msg.push_back('"');
    msg.append(filename);
    msg.push_back('"');
    perror(msg.data());
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  std::string track_graph_filename;
  std::string routes_filename;

  // processing arguments
  for (range(i, argc)) {
    if (i == 0) {
      continue;
    }

    if (strcmp(argv[i], "--track_graph") == 0 && i != argc - 1 &&
        track_graph_filename.empty()) {
      track_graph_filename.append(argv[i + 1]);
      i++;
    } else if (strcmp(argv[i], "--routes") == 0 && i != argc - 1 &&
               routes_filename.empty()) {
      routes_filename.append(argv[i + 1]);
      i++;
    } else {
      std::cout << "Bad argument " << '"' << argv[i] << '"' << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  if (track_graph_filename.empty()) {
    std::cout << "Enter track graph file name:";
    std::cin >> track_graph_filename;
  }

  std::ifstream track_graph_file = safeOpen(track_graph_filename);

  TrackGraph graph = TrackGraph::fromFile(track_graph_file);

  track_graph_file.close();

  if (routes_filename.empty()) {
    std::cout << "Enter routes file name:";
    std::cin >> routes_filename;
  }

  std::ifstream paths_file = safeOpen(routes_filename);

  std::vector<Route> routes;

  int paths_amount;
  paths_file >> paths_amount;
  for (range(i, paths_amount)) {
    routes.push_back(Route::fromFile(paths_file));
  }

  paths_file.close();

  for (range(i, paths_amount)) {
    if (!graph.isRouteValid(routes.at(i))) {
      std::cout << "Route " << i << " isn't valid in the track graph"
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  Schedule new_schedule(routes, {}, graph);
  new_schedule.solve();

  if (!new_schedule.isSolved()) {
    std::cout << "Scheduler found no feasible solutions." << std::endl;
    exit(EXIT_FAILURE);
  }

  for (range(i, new_schedule.getRoutesCount())) {
    Route route = new_schedule.getRoute(i);
    int64_t time = 0;
    for (range(j, (int)route.getLength())) {
      time += route.getVertex(j).min_time;
      std::cout << time << " " << route.getVertex(j).track << "\n";
    }
    std::cout << "\n";
  }

  auto res = graph.findAllRoutes(1, 20);

  for (auto r : res) {
    std::cout << "\n";
    for (uint32_t i = 0; i < r.getLength(); i++) {
      std::cout << r.getPosition(i) << " ";
    }
  }

  std::cout << std::endl;
  exit(EXIT_SUCCESS);
}
