#pragma once

#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/ObjectIdUtils.h"

using namespace uhh2;


class GenParticleIdSelection : public Selection {
public:
    explicit GenParticleIdSelection(const GenParticleId & id, int n_min, int n_max = -1);
    virtual bool passes(const Event &);
private:
    GenParticleId id_;
    int n_min_, n_max_;
};

class BoolSelection : public Selection {
public:
    explicit BoolSelection(Event::Handle<bool> const & hndl);
    virtual bool passes(const Event &);
private:
    Event::Handle<bool> hndl_;
};

class VetoSelection : public Selection {
public:
    explicit VetoSelection(std::shared_ptr<Selection> sel);
    virtual bool passes(const Event &);
private:
    std::shared_ptr<Selection> sel_;
};

class OrSelection: public Selection {
public:
    /// if cutflow_histo_name is empty, no histogram is created
    explicit OrSelection() {}
    
    /// add the selection module. The description is used for the cutflow histogram as x-axis label and in the summary printed at the end of the SFrame job.
    template<typename TSEL, typename... CARGS>
    void add(CARGS &&... cargs){
        // note: writing the add method this way instead of taking a pointer to Selection directly circumvents
        // the issues with memory ownership nicely.
        selections.emplace_back(new TSEL(std::forward<CARGS>(cargs)...));
    }

    /// add an already defined selection -> WARNING: if this is an OrSelection producing cutflow histograms, bear in mind that this might
    /// be filled twice in the event!
    void add(std::shared_ptr<Selection> sel){
        selections.push_back(sel);
    }
    
    // virtual ~OrSelection() {}
    
    virtual bool passes(const Event & event) override;
    
private:
    
    std::vector<std::shared_ptr<Selection> > selections;
};

class MttbarGenSelection : public Selection {
public:
  explicit MttbarGenSelection(double m_min, double m_max);
    virtual bool passes(const Event &);
private:
    double m_min_, m_max_;
};
