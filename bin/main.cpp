#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#define range(i, n)                                                            \
  int i = 0;                                                                   \
  i < n;                                                                       \
  i++

/*
 * This is a prototype application meant to perform checks and
 * create schedules.
 */

std::map<int, std::set<int>> graph;
std::map<int, std::set<int>> conflicts;
std::vector<std::vector<int>> paths;

int n; // number of tracks
int k; // number of paths

int max_len;

int main() {
  std::cout << "Enter track graph file name:";
  std::string filename;
  std::cin >> filename;

  std::ifstream track_graph_file = safeFileOpen(filename);

  track_graph_file >> n;
  for (range(i, n)) {
    inputTrack(track_graph_file);
  }

  track_graph_file.close();

  std::cout << "Enter paths file name:";
  std::cin >> filename;

  std::ifstream paths_file = safeFileOpen(filename);

  paths_file >> k;
  for (range(i, k)) {
    inputPath(paths_file);
  }

  paths_file.close();

  max_len =
      (int)(*std::max_element(paths.begin(), paths.end(), pathscmp)).size();

  for (range(j, k)) {
    if (!existsPath(paths.at(j))) {
      std::cout << "Path " << j << " does not exist in the graph.";
      return 1;
    }

    for (range(i, max_len)) {
      if (i < (int)paths.at(j).size()) {
        int res = checkForPosition(i, &j);
        max_len = (int)(*std::max_element(paths.begin(), paths.end(), pathscmp))
                      .size();

        if (res == 1) {
          return 1;
        } else if (res == -1) {
          j = 0;
          break;
        }
      }
    }
  }

  std::cout << "No problems have been found in the schedule.\n";

  for (auto i : paths) {
    for (auto j : i) {
      std::cout << j << " ";
    }
    std::cout << '\n';
  }

  std::cout << std::endl;
  return 0;
}
