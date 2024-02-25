#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <tuple>
#include <unistd.h>
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

bool pathscmp(std::vector<int> first, std::vector<int> second) {
  return first.size() < second.size();
}

void inputTrack(std::ifstream &file) {
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

  graph[track] = adjacent;
  conflicts[track] = conflicting;
}

void inputPath(std::ifstream &file) {
  int lenght;
  std::vector<int> path;

  file >> lenght;

  for (int i = 0; i < lenght; i++) {
    int element;
    file >> element;
    path.push_back(element);
  }
  paths.push_back(path);
}

//  Chechs is paths exists in the graph
bool existsPath(std::vector<int> path) {

  for (int i = 0; i < (int)path.size(); i++) {
    if (i > 0) {
      auto related = graph.find(path.at(i - 1));

      bool is_finished = (path.at(i) == -2);
      bool previous_vertex_has_related = (related != graph.end());
      if (!is_finished) {
        if (!previous_vertex_has_related) {
          return false;
        } else {
          bool is_previous_vertex_connected =
              related->second.find(path.at(i)) != related->second.end();
          bool is_moving = related->first != path.at(i);

          if (!is_previous_vertex_connected && is_moving) {
            return false;
          }
        }
      }
    }
  }
  return true;
}

int findPointToPush(std::vector<int> path, int i) {
  // std::cout << path.size() << "&" << i << "\n";
  int push_point = path.at(i);
  for (int p = i; p >= 0; p--) {
    // std::cout << "'" << p << "'\n";
    if (path.at(p) != push_point) {
      return p;
    }
  }
  return -1;
}

std::tuple<int, int, int> findOptimalPoint(std::vector<int> path1,
                                           std::vector<int> path2, int i) {
  int ptp1 = findPointToPush(path1, i);
  int ptp2 = findPointToPush(path2, i);
  int addp1 = -1;
  for (auto it = path1.begin() + i; it != path1.end(); ++it) {
    if (*it != path1.at(i)) {
      break;
    } else {
      addp1++;
    }
  }
  int addp2 = -1;
  for (auto it = path2.begin() + i; it != path2.end(); ++it) {
    if (*it != path2.at(i)) {
      break;
    } else {
      addp2++;
    }
  }

  if (i - ptp1 + addp2 <= i - ptp2 + addp1 || (ptp2 == -1 && ptp1 != -1)) {
    return std::make_tuple(ptp1, addp2, 0);
  } else {
    return std::make_tuple(ptp2, addp1, 1);
  }
}

int checkForPosition(int i, int *j) {
  for (range(k, (int)paths.size())) {
    if ((int)paths.at(k).size() <= i) {
      continue;
    }

    bool areConflicting =
        conflicts.find(paths.at(*j).at(i))->second.find(paths.at(k).at(i)) !=
            conflicts.find(paths.at(*j).at(i))->second.end() ||
        paths.at(k).at(i) == paths.at(*j).at(i);

    if (k != *j && areConflicting) {
      std::cout << "\nThe vertex " << i << "(" << paths.at(*j).at(i)
                << ") in path " << *j << " collides with " << paths.at(k).at(i)
                << " in path " << k << " and therefore makes the path invalid."
                << std::endl;
      std::cout << "Try adding a delay?[y/n]";
      char answer;
      std::cin >> answer;
      if (answer == 'y') {
        // std::cout << path1.size() << "/" << path2.size() << "\n";
        auto [push_point, add, which] =
            findOptimalPoint(paths.at(*j), paths.at(k), i);
        if (push_point == -1) {
          std::cout << "Unable to resolve this collision" << std::endl;
          return 1;
        }
        if (which == 0) {
          for (range(p, i - push_point + add)) {

            std::cout << "adding to " << *j << '\n';
            paths.at(*j).insert(paths.at(*j).begin() + push_point,
                                paths.at(*j).at(push_point));
          }
          // std::cout << paths.at(*j).size() << '\n';
        } else {
          for (range(p, i - push_point + add)) {
            std::cout << "adding to " << k << '\n';
            paths.at(k).insert(paths.at(k).begin() + push_point,
                               paths.at(k).at(push_point));
          }
        }
        return -1;
      } else {
        std::cout << "Exiting." << std::endl;
        return 1;
      }
    }
  }
  return 0;
}

std::ifstream safeFileOpen(std::string filename) {
  std::ifstream file;
  file.open(filename, std::ios::in);

  if (file.is_open()) {
    return file;
  } else {
    perror("Failed to open file");
    exit(1);
  }
}

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
