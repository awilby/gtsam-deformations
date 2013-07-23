/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    testSymbolicFactor.cpp
 * @brief   Unit tests for a symbolic IndexFactor
 * @author  Frank Dellaert
 */

#include <CppUnitLite/TestHarness.h>
#include <gtsam/base/TestableAssertions.h>
#include <gtsam/symbolic/SymbolicFactorUnordered.h>
#include <gtsam/symbolic/SymbolicConditionalUnordered.h>
#include <gtsam/symbolic/SymbolicFactorGraphUnordered.h>

#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/make_shared.hpp>
#include <boost/tuple/tuple.hpp>

using namespace std;
using namespace gtsam;
using namespace boost::assign;

/* ************************************************************************* */
#ifdef TRACK_ELIMINATE
TEST(SymbolicFactor, eliminate) {
  vector<Index> keys; keys += 2, 3, 4, 6, 7, 9, 10, 11;
  IndexFactor actual(keys.begin(), keys.end());
  BayesNet<IndexConditional> fragment = *actual.eliminate(3);

  IndexFactor expected(keys.begin()+3, keys.end());
  IndexConditional::shared_ptr expected0 = IndexConditional::FromRange(keys.begin(), keys.end(), 1);
  IndexConditional::shared_ptr expected1 = IndexConditional::FromRange(keys.begin()+1, keys.end(), 1);
  IndexConditional::shared_ptr expected2 = IndexConditional::FromRange(keys.begin()+2, keys.end(), 1);

  CHECK(assert_equal(fragment.size(), size_t(3)));
  CHECK(assert_equal(expected, actual));
  BayesNet<IndexConditional>::const_iterator fragmentCond = fragment.begin();
  CHECK(assert_equal(**fragmentCond++, *expected0));
  CHECK(assert_equal(**fragmentCond++, *expected1));
  CHECK(assert_equal(**fragmentCond++, *expected2));
}
#endif
/* ************************************************************************* */
TEST(SymbolicFactor, EliminateSymbolic)
{
  const SymbolicFactorGraphUnordered factors = list_of
    (SymbolicFactorUnordered(2,4,6))
    (SymbolicFactorUnordered(1,2,5))
    (SymbolicFactorUnordered(0,3));

  const SymbolicFactorUnordered expectedFactor(4,5,6);
  const SymbolicConditionalUnordered expectedConditional =
    SymbolicConditionalUnordered::FromKeys(list_of(0)(1)(2)(3)(4)(5)(6), 4);

  SymbolicFactorUnordered::shared_ptr actualFactor;
  SymbolicConditionalUnordered::shared_ptr actualConditional;
  boost::tie(actualConditional, actualFactor) =
    EliminateSymbolicUnordered(factors, list_of(0)(1)(2)(3));

  CHECK(assert_equal(expectedConditional, *actualConditional));
  CHECK(assert_equal(expectedFactor, *actualFactor));
}

/* ************************************************************************* */
int main() {
  TestResult tr;
  return TestRegistry::runAllTests(tr);
}
/* ************************************************************************* */
