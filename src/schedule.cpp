#include <cassert>
#include <cstdint>
#include <vector>

#include "route.h"
#include "schedule.h"
#include "track_graph.h"

#include "absl/strings/str_format.h"
#include "ortools/base/logging.h"
#include <ortools/sat/cp_model.h>
#include <ortools/sat/cp_model.pb.h>
#include <ortools/sat/cp_model_solver.h>

Schedule::Schedule(std::vector<Route> routes,
                   std::vector<std::vector<Route>> optroutes,
                   TrackGraph track_graph) {
  this->track_graph = track_graph;
  this->routes = routes;
  is_solved = false;
}

bool Schedule::isSolved() { return is_solved; }

Route Schedule::getRoute(int index) {
  assert(isSolved());
  return solution[index]; // here we return the LAST KNOWN good solution
}

int Schedule::getRoutesCount() { return solution.size(); }

void Schedule::solve() {
  // TODO: implement a function for constraint calculation

  // you will most likely get no syntax highlighting here, but that is OK, it
  // compiles just fine
  //
  // this code is mostly a copy of the google's guide on using ortools

  using namespace operations_research;
  using namespace sat; // operations_research::sat
  CpModelBuilder cp_model;

  // scheduling horizon is the last time something can happen in our model
  // we calculate it by summing all of the route lengths
  int64_t horizon = 0;
  for (auto route : routes) {
    horizon += route.getTime();
  }
  // dealing with the optroutes is slightly harder
  // we must compute the largest route out of them
  for (auto optroute : optroutes) {
    uint32_t max = 0;
    for (auto route : optroute) {
      max = (route.getTime() > max) ? route.getTime() : max;
    }
  }

  struct TaskType {
    // IntVar is not a variable, it is a type for the cp-sat, that IT decides to
    // change however it wants that is, it has no "value" in a more traditional
    // sense, instead it tells the sat how much it can cange this variable in
    // the solution. It is like telling the sat to "solve for x on interval (a,
    // b)"
    IntVar start;
    IntVar end;
    // interval is the same thing, but it has a size, meaning the sat has to
    // determine where to place an "interval" of some size
    IntervalVar interval;
  };

  using TaskID = std::tuple<int, int>; // (route id, track/task ID)
  auto all_tracks = track_graph.getTracks();
  std::map<TaskID, TaskType> all_tasks;
  std::map<int64_t, std::vector<IntervalVar>> tracks_intervals;

  /* tell cp-sat to define variables for our problem */
  for (int route_id = 0; route_id < routes.size(); ++route_id) {
    Route &route = routes[route_id];
    for (int task_id = 0; task_id < route.getLength(); ++task_id) {
      const auto [task, duration] = route.getVertex(task_id);
      std::string suffix = absl::StrFormat("_%d_%d", route_id, task_id);
      IntVar start = cp_model.NewIntVar({0, horizon})
                         .WithName(std::string("start") + suffix);
      IntVar end = cp_model.NewIntVar({0, horizon})
                       .WithName(std::string("end") + suffix);
      IntervalVar interval = cp_model.NewIntervalVar(start, duration, end)
                                 .WithName(std::string("interval") + suffix);
      TaskID key = std::make_tuple(route_id, task_id);
      all_tasks.emplace(key, TaskType{/*.start=*/start,
                                      /*.end=*/end,
                                      /*.interval=*/interval});
      tracks_intervals[task].push_back(interval);
    }
  }

  for (const auto track : all_tracks) {
    cp_model.AddNoOverlap(tracks_intervals[track]);
  }

  for (int route_id = 0; route_id < routes.size(); ++route_id) {
    Route &route = routes[route_id];
    for (int task_id = 0; task_id < route.getLength() - 1; ++task_id) {
      TaskID key = std::make_tuple(route_id, task_id);
      TaskID next_key = std::make_tuple(route_id, task_id + 1);
      cp_model.AddGreaterOrEqual(all_tasks[next_key].start, all_tasks[key].end);
    }
  }

  IntVar obj_var = cp_model.NewIntVar({0, horizon}).WithName("makespan");

  std::vector<IntVar> ends;
  for (int job_id = 0; job_id < routes.size(); ++job_id) {
    auto &job = routes[job_id];
    TaskID key = std::make_tuple(job_id, job.getLength() - 1);
    ends.push_back(all_tasks[key].end);
  }
  cp_model.AddMaxEquality(obj_var, ends);
  cp_model.Minimize(obj_var);

  const CpSolverResponse response = Solve(cp_model.Build());
  is_solved = (response.status() == CpSolverStatus::OPTIMAL ||
               response.status() == CpSolverStatus::FEASIBLE);

  if (is_solved) {
    solution.clear(); // delete the last solution

    for (int job_id = 0; job_id < routes.size(); ++job_id) {
      auto &job = routes[job_id];
      std::vector<routeVertex> route;
      for (int task_id = 0; task_id < job.getLength() - 1; ++task_id) {

        const auto machine = job.getPosition(task_id);
        TaskID key = std::make_tuple(job_id, task_id);
        int64_t start = SolutionIntegerValue(
            response, all_tasks[std::make_tuple(job_id, task_id)].start);
        int64_t end = SolutionIntegerValue(
            response, all_tasks[std::make_tuple(job_id, task_id + 1)].start);
        int64_t duration = end - start;

        route.push_back(routeVertex{machine, duration});
      }
      solution.push_back(Route(route));
    }
  }
}
