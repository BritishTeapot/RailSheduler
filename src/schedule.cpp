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

Schedule::Schedule(std::vector<Route> routes, TrackGraph track_graph) {
  this->track_graph = track_graph;
  this->routes = routes;
}

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
    horizon += route.getLength() * 60; // HOTFIX for no constraint function
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

  auto all_machines = track_graph.getTracks();

  using TaskID = std::tuple<int, int>; // (route id, track/task ID)
                                       // std::map<TaskID, TaskType> all_tasks;
  std::map<TaskID, TaskType> all_tasks;
  std::map<int64_t, std::vector<IntervalVar>> machine_to_intervals;
  for (int job_id = 0; job_id < routes.size(); ++job_id) {
    Route &job = routes[job_id];
    for (int task_id = 0; task_id < job.getLength(); ++task_id) {
      const auto task = job.getPosition(task_id);
      std::string suffix = absl::StrFormat("_%d_%d", job_id, task_id);
      IntVar start = cp_model.NewIntVar({0, horizon})
                         .WithName(std::string("start") + suffix);
      IntVar end = cp_model.NewIntVar({0, horizon})
                       .WithName(std::string("end") + suffix);
      IntervalVar interval =
          cp_model
              .NewIntervalVar(
                  start,
                  60, /* 60 is a HOTFIX for abscense of a constraint function */
                  end)
              .WithName(std::string("interval") + suffix);
      TaskID key = std::make_tuple(job_id, task_id);
      all_tasks.emplace(key, TaskType{/*.start=*/start,
                                      /*.end=*/end,
                                      /*.interval=*/interval});
      machine_to_intervals[task].push_back(interval);
    }
  }

  for (const auto machine : all_machines) {
    cp_model.AddNoOverlap(machine_to_intervals[machine]);
  }

  for (int job_id = 0; job_id < routes.size(); ++job_id) {
    auto &job = routes[job_id];
    for (int task_id = 0; task_id < job.getLength() - 1; ++task_id) {
      TaskID key = std::make_tuple(job_id, task_id);
      TaskID next_key = std::make_tuple(job_id, task_id + 1);
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

  // vvvvvvvvvvvvvvvvvvv REMOVE LATER vvvvvvvvvvvvvvvvvvvv

  if (response.status() == CpSolverStatus::OPTIMAL ||
      response.status() == CpSolverStatus::FEASIBLE) {
    LOG(INFO) << "Solution:";
    // create one list of assigned tasks per machine.
    struct AssignedTaskType {
      int job_id;
      int task_id;
      int64_t start;
      int64_t duration;

      bool operator<(const AssignedTaskType &rhs) const {
        return std::tie(this->start, this->duration) <
               std::tie(rhs.start, rhs.duration);
      }
    };

    std::map<int64_t, std::vector<AssignedTaskType>> assigned_jobs;
    for (int job_id = 0; job_id < routes.size(); ++job_id) {
      auto &job = routes[job_id];
      for (int task_id = 0; task_id < job.getLength(); ++task_id) {
        const auto machine = job.getPosition(task_id);
        TaskID key = std::make_tuple(job_id, task_id);
        int64_t start = SolutionIntegerValue(response, all_tasks[key].start);
        assigned_jobs[machine].push_back(AssignedTaskType{
            /*.job_id=*/job_id,
            /*.task_id=*/task_id,
            /*.start=*/start,
            /*.duration=*/60}); // HOTFIX for abscense of a function
      }
    }

    // Create per machine output lines.
    std::string output = "";
    for (const auto machine : all_machines) {
      // Sort by starting time.
      std::sort(assigned_jobs[machine].begin(), assigned_jobs[machine].end());
      std::string sol_line_tasks = "Machine " + std::to_string(machine) + ": ";
      std::string sol_line = "           ";

      for (const auto &assigned_task : assigned_jobs[machine]) {
        std::string name = absl::StrFormat(
            "job_%d_task_%d", assigned_task.job_id, assigned_task.task_id);
        // Add spaces to output to align columns.
        sol_line_tasks += absl::StrFormat("%-15s", name);

        int64_t start = assigned_task.start;
        int64_t duration = assigned_task.duration;
        std::string sol_tmp =
            absl::StrFormat("[%i,%i]", start, start + duration);
        // Add spaces to output to align columns.
        sol_line += absl::StrFormat("%-15s", sol_tmp);
      }
      output += sol_line_tasks + "\n";
      output += sol_line + "\n";
    }
    // Finally print the solution found.
    LOG(INFO) << "Optimal Schedule Length: " << response.objective_value();
    LOG(INFO) << "\n" << output;
  } else {
    LOG(INFO) << "No solution found.";
  }

  // ^^^^^^^^^^ REMOVE LATER END ^^^^^^^^^^^^^^^^^
}
