#pragma once

#include <boost/optional.hpp>
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"

// This file defines AnalysisModules which calculate relatively simple
// global event variables which are used by many analysis, such as HT, the
// scalar sum of all jet's pt.


/** \brief Calculate HT and write the result to the event
 *
 * The result is written to the event as double with the name
 * as given in the constructor, which defaults to "HT".
 *
 * HT is the scalar sum of all jet ids which pass the given jetid.
 * If no jetid is specified, all jets which are currently in the event
 * are used, so any cut which is currently applied is used implicitly.
 */
class HTCalculator: public uhh2::AnalysisModule {
public:
    explicit HTCalculator(uhh2::Context & ctx, const boost::optional<JetId> & jetid = boost::none, const std::string & name = "HT");

    virtual bool process(uhh2::Event & event) override;

private:
    boost::optional<JetId> jetid;
    uhh2::Event::Handle<double> h_ht;
};


