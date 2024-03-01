#include <fstream>
#include <vector>

class Route {
private:
  std::vector<int> path;

public:
  Route(std::vector<int> path);
  int getPosition(int index);
  static Route fromFile(std::ifstream &file);
};
