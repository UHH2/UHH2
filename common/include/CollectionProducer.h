#pragma once

#include <vector>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

using namespace std;
using namespace uhh2;


/** \brief produce a new collection of objects, given an id-function/tagger.
 *
 * When given a tagger or id function and an input collection, this module
 * produces a new collection. Below is an example on producing a collection of
 * b-jets, but it can also be applied to electrons, muons, or whatever is stored
 * via Event::Handle<std::vector<WHATEVER>>.
 *
 * Usage (how to produce b-jets):
 * \code
 * // in the AnalysisModule constructor:
 * // assuming a 'std::unique_ptr<AnalysisModule> cp' as member varibale
 * cp.reset(new CollectionProducer<Jet>(
 *     context,
 *     CSVBTag(WP_MEDIUM), // from common/include/JetIds.h
 *     "jets",
 *     "b_jets"
 * ));
 *
 * // in AnalysisModule::process:
 * cp->process(event);
 * // now the Handle<Jet> to "b_jets" is valid
 *
 * \endcode
 */
template <typename TYPE>
class CollectionProducer: public AnalysisModule {
public:
    typedef std::function<bool (const TYPE &, const uhh2::Event &)> TYPE_ID_FUNC;

    explicit CollectionProducer(Context & ctx,
                                TYPE_ID_FUNC const & type_id,
                                std::string const & in_name,
                                std::string const & out_name):
        in_hndl(ctx.get_handle<vector<TYPE>>(in_name)),
        out_hndl(ctx.get_handle<vector<TYPE>>(out_name)),
        type_id_(type_id) {}

    bool process(Event & event) override {
        vector<TYPE> out_coll;
        for(const TYPE & obj : event.get(in_hndl)){
            if (type_id_(obj, event)) {
                out_coll.push_back(obj);
            }
        }
        event.set(out_hndl, out_coll);
        return true;
    }

private:
    Event::Handle<vector<TYPE>> in_hndl;
    Event::Handle<vector<TYPE>> out_hndl;
    TYPE_ID_FUNC type_id_;
};  // class CollectionProducer


/** \brief produce the number tagged objects, given an id-function/tagger.
 *
 * When given a tagger or id function and an input collection, this module
 * produces an int, stating for how many objects the id-function evaluates to
 * true. Below is an example on producing the number of b-jets, but it can also
 * be applied to electrons, muons, or whatever is stored via
 * Event::Handle<std::vector<WHATEVER>>.
 *
 * Usage (how to produce b-jets):
 * \code
 * // in the AnalysisModule constructor:
 * // assuming a 'std::unique_ptr<AnalysisModule> cp' as member varibale
 * cp.reset(new CollectionProducer<Jet>(
 *     context,
 *     CSVBTag(WP_MEDIUM), // from common/include/JetIds.h
 *     "jets",
 *     "n_btags"
 * ));
 *
 * // in AnalysisModule::process:
 * cp->process(event);
 * // now the Handle<int> to "n_btags" is valid
 *
 * \endcode
 */
template <typename TYPE>
class CollectionSizeProducer: public AnalysisModule {
public:
    typedef std::function<bool (const TYPE &, const uhh2::Event &)> TYPE_ID_FUNC;

    explicit CollectionSizeProducer(Context & ctx,
                           TYPE_ID_FUNC const & type_id,
                           std::string const & in_name,
                           std::string const & out_name):
        in_hndl(ctx.get_handle<vector<TYPE>>(in_name)),
        out_hndl(ctx.get_handle<int>(out_name)),
        type_id_(type_id) {}

    bool process(Event & event) override {
        int n_obj = 0;
        for(const TYPE & obj : event.get(in_hndl)){
            if (type_id_(obj, event)) {
                n_obj++;
            }
        }
        event.set(out_hndl, n_obj);
        return true;
    }

private:
    Event::Handle<vector<TYPE>> in_hndl;
    Event::Handle<int> out_hndl;
    TYPE_ID_FUNC type_id_;
};  // class CollectionSizeProducer
