#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

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
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }
}

int main() {
  std::cout << "Enter track graph file name:";
  std::string filename;
  std::cin >> filename;

  std::ifstream track_graph_file = safeOpen(filename);

  TrackGraph graph = TrackGraph::fromFile(track_graph_file);

  track_graph_file.close();

  std::cout << "Enter paths file name:";
  std::cin >> filename;

  std::ifstream paths_file = safeOpen(filename);

  std::vector<Route> routes;

  int paths_amount;
  paths_file >> paths_amount;
  for (range(i, paths_amount)) {
    routes.push_back(Route::fromFile(paths_file));
  }

  paths_file.close();

  for (range(i, paths_amount)) {
    graph.isRouteValid(routes.at(i));
  }

  // TODO: job-shop check here

  std::cout << "No problems have been found in the schedule.\n";

  /*
  for (auto i : paths) {
    for (auto j : i) {
      std::cout << j << " ";
    }
    std::cout << '\n';
  }
  */

  std::cout << std::endl;
  return 0;
}
