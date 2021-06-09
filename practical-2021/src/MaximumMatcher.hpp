#pragma once
#include <iostream>
#include <vector>
#include "ortools/graph/assignment.h"
#include <utility>

class MaximumMatcher {
public:
  static int convert_weight(double weight) {
    return static_cast<int> ((-256 * weight) + 0.5);
  }
  static std::vector<size_t> match_vector(const std::vector<std::vector<double>>& weights) {
    std::vector<size_t> matching_vector(weights.size());
    operations_research::SimpleLinearSumAssignment assignment;
    for (size_t i = 0; i < weights.size(); ++i) {
      for(size_t j = 0; j < weights[i].size(); ++j) {
        assignment.AddArcWithCost(static_cast<int>(i), static_cast<int>(j),  convert_weight(weights[i][j])); //@Softwipe, added explicit conversion
        //Might not be the solution
      }
    }
    if (assignment.Solve() == operations_research::SimpleLinearSumAssignment::OPTIMAL) {
      //printf("A perfect matching exists.\n");
      //printf("The best possible cost is %d.\n", assignment.OptimalCost());
      //printf("An optimal assignment is:\n");
      size_t num_nodes = static_cast<size_t> (assignment.NumNodes());
      for (size_t node = 0; node < num_nodes; ++node) {

        /*printf("left node %d assigned to right node %d with cost %i (%i).\n",
        node,
        assignment.RightMate(node),
        (- assignment.AssignmentCost(node)),
        assignment.AssignmentCost(node));*/
        matching_vector[node] = assignment.RightMate(static_cast<size_t>(node));
        //std::cout << "Result(Node): " << node << "->" << assignment.RightMate(node) << " = " << result << "\n";
      }
      //printf("Note that it may not be the unique optimal assignment.");
    }
    return matching_vector;
  }
  static double match(const std::vector<std::vector<double>>& weights) {
    std::vector<size_t> matching = match_vector(weights);
    assert(matching.size() == weights.size());
    double result = 0;
    for(size_t i = 0; i < matching.size(); ++i) {
      result += weights[i][matching[i]];
    }
    return result;
  }

};
