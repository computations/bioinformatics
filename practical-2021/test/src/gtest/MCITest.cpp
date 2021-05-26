#include "gtest/gtest.h"
#include "../../../src/datastructures/PllSplits.hpp"
#include "../../../src/metrics/MCI.hpp"
#include "../../../src/PhylogeneticMathUtils.hpp"
#include "../TestUtil.hpp"
class MCITest : public testing::Test {};


//This is a testcase from the old Metricstest that contained MCI, it is here now but needs a better name
TEST_F(MCITest, test_clustering_probability) {
  PllSplit::setTipCount(12);
  EXPECT_DOUBLE_EQ(phylomath::clusteringProbability(4), 1.0d/3);
  std::vector<size_t> part1_a = {0, 1, 2, 3, 4};
  PllSplit split_a = TestUtil::createSplit(part1_a);
  std::vector<size_t> part1_b = {0, 3, 4, 5, 6, 7};
  PllSplit split_b = TestUtil::createSplit(part1_b);
  double solution = ((1.0d / 4) * std::log(6.0d / 5)) + ((1.0d / 6) * std::log(4.0d / 5)) +
    ((1.0d / 4) * std::log(6.0d / 7)) + ((1.0d / 3) * std::log(8.0d / 7));
  MCI metric_mci;  
  EXPECT_NEAR(metric_mci.evaluate(split_a, split_b), solution, 0.00000000001);
  free(split_a());
  free(split_b());
}
TEST_F(MCITest, test_identity) {
  PllSplit::setTipCount(6);
  std::vector<size_t> part1 = {0, 3, 4};
  PllSplit split = TestUtil::createSplit(part1);
  MCI metric_mci;
  double p_a = phylomath::clusteringProbability(split, Block_A);
  double p_b = phylomath::clusteringProbability(split, Block_B);
  EXPECT_DOUBLE_EQ(metric_mci.evaluate(split, split), p_a * std::log(6.0d/3) + p_b * std::log(6.0d/3));
  free(split());
}

