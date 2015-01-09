#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/core/test/common.h"

#include <iostream>

using namespace uhh2;
using namespace std;

// metadata tests
// A: on-the-fly producer and consumer
// B: producer only, to disk
// C: consumer only, from disk (from B)
// D: producer producing the same as from disk, and 2 metadata consumers: one from disk, one via handle

class MetaDataProducerA: public AnalysisModule {
public:
    MetaDataProducerA(Context & ctx){
        meta = ctx.create_metadata<double>("testdata", false);

        // test that trying to create metadata with same name twice triggers
        // error:
        bool exception = false;
        try {
            ctx.create_metadata<float>("testdata", false);
        }
        catch(std::exception & ex){
            exception = true;
        }
        assert(exception);
    }
    
    virtual bool process(Event &) override {
        if(ievent % 10 == 0){
            meta = ievent;
        }
        ++ievent;
        return true;
    }
    
private:
    Context::MetadataObject<double> meta;
    int ievent = 0;
};

class MetaDataConsumerA: public AnalysisModule {
public:
    MetaDataConsumerA(Context & ctx){
        ctx.register_metadata_callback<double>("testdata", [this](const double & d){ this->on_value_changed(d);});
    }

    void on_value_changed(const double & newval){
        value = newval;
    }

    virtual bool process(Event &) override {
        assert(value);
        double expected_value = ievent / 10 * 10;
        assert(*value == expected_value);
        ++ievent;
        return true;
    }
private:
    int ievent = 0;
    boost::optional<double> value;

};

class MetaDataA: public AnalysisModule {
public:
    MetaDataA(Context & ctx){
        modules.emplace_back(new MetaDataProducerA(ctx));
        modules.emplace_back(new MetaDataConsumerA(ctx));
    }

    virtual bool process(Event & e) override {
        for(auto & m : modules){
            m->process(e);
        }
        return true;
    }

private:
    std::vector<std::unique_ptr<AnalysisModule>> modules;
};


UHH2_REGISTER_ANALYSIS_MODULE(MetaDataA)


class MetaDataB: public AnalysisModule {
public:
    MetaDataB(Context & ctx){
        meta = ctx.create_metadata<double>("testdata", true);
    }

    virtual bool process(Event &) override {
        if(ievent % 100 == 0){
            meta = ievent;
        }
        ievent++;
        return true;
    }

private:
    int ievent = 0;
    Context::MetadataObject<double> meta;
};


UHH2_REGISTER_ANALYSIS_MODULE(MetaDataB)

class MetaDataC: public AnalysisModule {
public:
    MetaDataC(Context & ctx){
        ctx.register_metadata_callback<double>("testdata", [this](const double & val){ this->set_testdata_value(val); });
    }
    
    void set_testdata_value(double val){
        testdata_value = val;
        //cout << "MetaDataC at ievent " << ievent << ": received testdata value: " << val << endl;
    }

    virtual bool process(Event &) override {
        double expected_testdata_value = ievent / 100 * 100;
        // check that it has been set before we go to process:
        assert(testdata_value);
        assert(*testdata_value == expected_testdata_value);
        ++ievent;
        return true;
    }

private:
    int ievent = 0;
    boost::optional<double> testdata_value;
};


UHH2_REGISTER_ANALYSIS_MODULE(MetaDataC)


class MetaDataD: public AnalysisModule {
public:

    MetaDataD(Context & ctx){
        ctx.register_metadata_callback<double>("testdata", [this](const double & val){ this->set_testdata_value_disk(val); }, Context::metadata_source_policy::infile_then_handle);
        ctx.register_metadata_callback<double>("testdata", [this](const double & val){ this->set_testdata_value_onfly(val); });
        md = ctx.create_metadata<double>("testdata", false);
    }
    
    void set_testdata_value_disk(double val){
        testdata_value_disk = val;
        //cout << "MetaDataD at ievent " << ievent << ": received disk testdata value: " << val << endl;
    }
    
    void set_testdata_value_onfly(double val){
        testdata_value_onfly = val;
        //cout << "MetaDataD at ievent " << ievent << ": received onfly testdata value: " << val << endl;
    }

    virtual bool process(Event &) override {
        double expected_testdata_value_onfly = -ievent / 100 * 100;
        if(ievent % 100 == 0){
            md = expected_testdata_value_onfly; // should trigger callback ...
        }
        double expected_testdata_value_disk = ievent / 100 * 100;
        // check that it has been set before we go to process:
        assert(testdata_value_disk);
        assert(testdata_value_onfly);
        assert(*testdata_value_disk == expected_testdata_value_disk);
        assert(*testdata_value_onfly == expected_testdata_value_onfly);
        ++ievent;
        return true;
    }

private:
    int ievent = 0;
    Context::MetadataObject<double> md;
    boost::optional<double> testdata_value_disk;
    boost::optional<double> testdata_value_onfly;
};


UHH2_REGISTER_ANALYSIS_MODULE(MetaDataD)


class KeepEvery: public AnalysisModule {
public:

    KeepEvery(Context & ctx){
        n = size_t(string2double(ctx.get("N", "2")));
        assert(n > 0);
    }

    virtual bool process(Event &) override {
        ++ievent;
        return ievent % n == 0;
    }

private:
    int ievent = -1;
    int n;
};

UHH2_REGISTER_ANALYSIS_MODULE(KeepEvery)
