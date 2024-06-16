#include <cassert>
#include <cstdint>
#include <queue>
#include <set>
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
  this->optroutes = optroutes;
  this->routes = routes;
  is_solved = false;
}

bool Schedule::isSolved() { return is_solved; }

Route Schedule::getRoute(int index) {
  assert(isSolved());
  return solution.at(index); // here we return the LAST KNOWN good solution
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
    horizon += max;
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

  for (auto t : all_tracks) {
    tracks_intervals.insert({t, {}});
  }

  /* tell cp-sat to define variables for our problem */
  for (int route_id = 0; route_id < routes.size(); route_id++) {
    Route &route = routes.at(route_id);
    for (int task_id = 0; task_id < route.getLength(); task_id++) {
      const auto [task, duration] = route.getVertex(task_id);
      std::string suffix = absl::StrFormat("_%d_%d", route_id, task_id);
      IntVar start = cp_model.NewIntVar({0, horizon})
                         .WithName(std::string("start") + suffix);
      IntVar end = cp_model.NewIntVar({0, horizon})
                       .WithName(std::string("end") + suffix);
      IntervalVar interval = cp_model.NewIntervalVar(start, duration + 1, end)
                                 .WithName(std::string("interval") + suffix);
      TaskID key = std::make_tuple(route_id, task_id);
      all_tasks.emplace(key, TaskType{/*.start=*/start,
                                      /*.end=*/end,
                                      /*.interval=*/interval});
      tracks_intervals.at(task).push_back(interval);
    }
  }

  using OptTaskID =
      std::tuple<int, int, int>;           // (route id, opt id, track/task ID)
  using OptRouteID = std::tuple<int, int>; // (route id, opt id)

  auto all_opt_tracks = track_graph.getTracks();
  std::map<OptRouteID, BoolVar> opt_route_presence;
  std::map<OptTaskID, TaskType> all_opt_tasks;

  for (int route_id = routes.size();
       route_id < routes.size() + optroutes.size(); route_id++) {
    auto optroute = optroutes.at(route_id - routes.size());
    for (int opt_id = 0; opt_id < optroute.size(); opt_id++) {
      Route &route = optroute.at(opt_id);
      BoolVar presence = cp_model.NewBoolVar().WithName(
          "presence" + absl::StrFormat("_%d_%d", route_id, opt_id));
      opt_route_presence.insert({{route_id, opt_id}, presence});
      for (int task_id = 0; task_id < route.getLength(); ++task_id) {
        const auto [track, duration] = route.getVertex(task_id);
        std::string suffix =
            absl::StrFormat("_%d_%d_%d", route_id, opt_id, task_id);
        IntVar start = cp_model.NewIntVar({0, horizon})
                           .WithName(std::string("start") + suffix);
        IntVar end = cp_model.NewIntVar({0, horizon})
                         .WithName(std::string("end") + suffix);
        IntervalVar interval =
            cp_model.NewOptionalIntervalVar(start, duration + 1, end, presence)
                .WithName(std::string("interval") + suffix);
        OptTaskID key = std::make_tuple(route_id, opt_id, task_id);
        all_opt_tasks.emplace(key, TaskType{start, end, interval});
        tracks_intervals.at(track).push_back(interval);
      }
    }
  }

  /* no interval overlaps on conflicting tracks */
  for (const auto track : all_tracks) {
    auto conflicting = tracks_intervals.at(track);

    for (auto ctrack : track_graph.getConflicting(track)) {
      for (auto i : tracks_intervals.at(ctrack)) {
        conflicting.push_back(i);
      }
    }

    cp_model.AddNoOverlap(conflicting);
  }

  /* consecutive intervals in routes */
  for (int route_id = 0; route_id < routes.size(); route_id++) {
    Route &route = routes.at(route_id);
    for (int task_id = 0; task_id < route.getLength() - 1; task_id++) {
      TaskID key = std::make_tuple(route_id, task_id);
      TaskID next_key = std::make_tuple(route_id, task_id + 1);
      cp_model.AddGreaterOrEqual(all_tasks.at(next_key).start + LinearExpr(1),
                                 all_tasks.at(key).end);
    }
  }

  for (int route_id = routes.size();
       route_id < routes.size() + optroutes.size(); route_id++) {
    auto optroute = optroutes.at(route_id - routes.size());
    for (int opt_id = 0; opt_id < optroute.size(); opt_id++) {
      Route &route = optroute.at(opt_id);
      OptRouteID rid = {route_id, opt_id};

      for (int task_id = 0; task_id < route.getLength() - 1; ++task_id) {
        OptTaskID key = std::make_tuple(route_id, opt_id, task_id);
        OptTaskID next_key = std::make_tuple(route_id, opt_id, task_id + 1);
        cp_model
            .AddGreaterOrEqual(all_opt_tasks.at(next_key).start,
                               all_opt_tasks.at(key).end)
            .OnlyEnforceIf(opt_route_presence.at(rid));
      }
    }
  }

  /* at least one opt route must be chosen */
  for (int route_id = routes.size();
       route_id < routes.size() + optroutes.size(); route_id++) {
    auto optroute = optroutes.at(route_id - routes.size());
    std::vector<BoolVar> all_presences;
    for (int opt_id = 0; opt_id < optroute.size(); opt_id++) {
      OptRouteID rid = {route_id, opt_id};
      all_presences.push_back(opt_route_presence.at(rid));
    }

    if (all_presences.size() != 0)
      cp_model.AddExactlyOne(all_presences);
  }

  IntVar obj_var = cp_model.NewIntVar({0, horizon}).WithName("makespan");

  LinearExpr ends;
  int count = 0;
  for (int route_id = 0; route_id < routes.size(); route_id++) {
    auto &route = routes.at(route_id);
    TaskID key = std::make_tuple(route_id, route.getLength() - 1);
    ends += (LinearExpr(route.getTime()) - all_tasks.at(key).end);
    count++;
  }
  for (int route_id = routes.size();
       route_id < routes.size() + optroutes.size(); route_id++) {
    auto optroute = optroutes.at(route_id - routes.size());
    uint32_t max = 0;
    for (auto route : optroute) {
      max = (route.getTime() > max) ? route.getTime() : max;
    }
    for (int opt_id = 0; opt_id < optroute.size(); opt_id++) {
      OptRouteID rid = {route_id, opt_id};
      auto &route = optroute.at(opt_id);
      OptTaskID key = std::make_tuple(route_id, opt_id, route.getLength() - 1);
      cp_model.AddGreaterOrEqual(all_opt_tasks.at(key).end, LinearExpr(max));
      ends += (LinearExpr(max) - all_opt_tasks.at(key).end);
      count++;
    }
  }
  cp_model.AddAbsEquality(obj_var, ends);
  /*
   * Originally this was meant to be average of the total delay
   * It turns out however, it makes no difference whether you make it avrage or
   * not.
   */
  // cp_model.Minimize(LinearExpr::WeightedSum({obj_var}, {count}));
  cp_model.Minimize(obj_var);

  const CpSolverResponse response = Solve(cp_model.Build());
  is_solved = (response.status() == CpSolverStatus::OPTIMAL ||
               response.status() == CpSolverStatus::FEASIBLE);

  if (is_solved) {
    solution.clear(); // delete the last solution

    for (int route_id = 0; route_id < routes.size(); ++route_id) {
      auto &route = routes.at(route_id);
      std::vector<routeVertex> new_route;
      for (int task_id = 0; task_id < route.getLength(); ++task_id) {

        const auto machine = route.getPosition(task_id);
        TaskID key = std::make_tuple(route_id, task_id);
        int64_t start = SolutionIntegerValue(
            response, all_tasks.at(std::make_tuple(route_id, task_id)).start);
        int64_t end;
        if (task_id == route.getLength() - 1) {
          end = SolutionIntegerValue(
              response, all_tasks.at(std::make_tuple(route_id, task_id)).end);

        } else {
          end = SolutionIntegerValue(
              response,
              all_tasks.at(std::make_tuple(route_id, task_id + 1)).start);
        }
        int64_t duration = end - start;

        new_route.push_back(routeVertex{machine, duration});
      }

      solution.push_back(Route(new_route));
    }
    for (int route_id = routes.size();
         route_id < routes.size() + optroutes.size(); ++route_id) {
      auto optroute = optroutes.at(route_id - routes.size());
      for (int opt_id = 0; opt_id < optroute.size(); opt_id++) {
        OptRouteID rid = {route_id, opt_id};
        if (SolutionBooleanValue(response, opt_route_presence.at(rid))) {
          Route &route = optroute.at(opt_id);
          std::vector<routeVertex> new_route;
          for (int task_id = 0; task_id < route.getLength(); ++task_id) {

            const auto machine = route.getPosition(task_id);
            OptTaskID key = std::make_tuple(route_id, opt_id, task_id);
            int64_t start =
                SolutionIntegerValue(response, all_opt_tasks.at(key).start);
            int64_t end;
            if (task_id == route.getLength() - 1) {
              end = SolutionIntegerValue(response, all_opt_tasks.at(key).end);

            } else {
              end = SolutionIntegerValue(
                  response,
                  all_opt_tasks
                      .at(std::make_tuple(route_id, opt_id, task_id + 1))
                      .start);
            }
            int64_t duration = end - start;

            new_route.push_back(routeVertex{machine, duration});
          }

          solution.push_back(Route(new_route));
        }
      }
    }
  }
}
