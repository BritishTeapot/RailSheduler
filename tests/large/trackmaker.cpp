

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

std::fstream safeOpen(std::string filename) {
  std::fstream file;
  file.open(filename, std::ios::in | std::ios::out | std::ios::app);

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

int main() {
  using namespace std;
  string filename = "track_graph_gen.txt";
  auto file = safeOpen(filename);

  while (true) {
    int64_t track;
    cout << "\ntrack:";
    cin >> track;
    if (track == 0) {
      break;
    }
    track *= 2;

    vector<int> conflicting;
    cout << "\nconflicting[count first]:";
    int c;
    cin >> c;
    for (int i = 0; i < c; i++) {
      int64_t to;
      cin >> to;
      to = to * 2;
      conflicting.push_back(to);
      conflicting.push_back(to - 1);
    }

    file << "\n" << track - 1;

    cout << "\nfrom right[count first]:";
    cin >> c;
    file << "\n" << c;
    for (int i = 0; i < c; i++) {
      int64_t to;
      cin >> to;
      to = to * 2 - 1;
      file << " " << to;
    }

    file << "\n" << conflicting.size();
    for (auto i : conflicting) {
      file << " " << i;
    }

    file << "\n";

    file << "\n" << track;
    cout << "\nfrom left[count first]:";
    cin >> c;
    file << "\n" << c;
    for (int i = 0; i < c; i++) {
      int64_t to;
      cin >> to;
      to = to * 2;
      file << " " << to;
    }

    file << "\n" << conflicting.size();
    for (auto i : conflicting) {
      file << " " << i;
    }

    file << "\n";
  }

  file.close();
}
