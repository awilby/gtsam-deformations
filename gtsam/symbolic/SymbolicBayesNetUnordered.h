/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file SymbolicBayesNet.h
 * @date Oct 29, 2009
 * @author Frank Dellaert
 * @author Richard Roberts
 */

#pragma once

#include <gtsam/symbolic/SymbolicConditionalUnordered.h>
#include <gtsam/inference/FactorGraphUnordered.h>
#include <gtsam/base/types.h>

namespace gtsam {

  /** Symbolic Bayes Net
   *  \nosubgrouping
   */
  class GTSAM_EXPORT SymbolicBayesNetUnordered : public FactorGraphUnordered<SymbolicConditionalUnordered> {

  public:

    typedef FactorGraphUnordered<SymbolicConditionalUnordered> Base;
    typedef SymbolicBayesNetUnordered This;
    typedef SymbolicConditionalUnordered ConditionalType;
    typedef boost::shared_ptr<This> shared_ptr; 
    typedef boost::shared_ptr<ConditionalType> sharedConditional;

    /// @name Standard Constructors
    /// @{

    /** Construct empty factor graph */
    SymbolicBayesNetUnordered() {}

    /** Construct from iterator over conditionals */
    template<typename ITERATOR>
    SymbolicBayesNetUnordered(ITERATOR firstConditional, ITERATOR lastConditional) : Base(firstConditional, lastConditional) {}

    /** Construct from container of factors (shared_ptr or plain objects) */
    template<class CONTAINER>
    explicit SymbolicBayesNetUnordered(const CONTAINER& conditionals) : Base(conditionals) {}

    /** Implicit copy/downcast constructor to override explicit template container constructor */
    template<class DERIVEDCONDITIONAL>
    SymbolicBayesNetUnordered(const FactorGraphUnordered<DERIVEDCONDITIONAL>& graph) : Base(graph) {}
    
    /// @}

    /// @name Testable
    /// @{
    
    /** Check equality */
    bool equals(const This& bn, double tol = 1e-9) const;

    /// @}
    
    /// @name Standard Interface
    /// @{
    
    void saveGraph(const std::string &s, const KeyFormatter& keyFormatter = DefaultKeyFormatter) const;
    
    /// @}
  };

} // namespace gtsam
