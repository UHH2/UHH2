#include "UHH2/JetMETObjects/interface/JetResolution.h"

namespace JME {

    JetResolution::JetResolution(const std::string& filename) {
        m_object = std::make_shared<JetResolutionObject>(filename);
    }

    JetResolution::JetResolution(const JetResolutionObject& object) {
        m_object = std::make_shared<JetResolutionObject>(object);
    }

    float JetResolution::getResolution(const JetParameters& parameters) const {
        const JetResolutionObject::Record* record = m_object->getRecord(parameters);
        if (! record)
            return 1;

        return m_object->evaluateFormula(*record, parameters);
    }

    JetResolutionScaleFactor::JetResolutionScaleFactor(const std::string& filename) {
        m_object = std::make_shared<JetResolutionObject>(filename);
    }

    JetResolutionScaleFactor::JetResolutionScaleFactor(const JetResolutionObject& object) {
        m_object = std::make_shared<JetResolutionObject>(object);
    }

    float JetResolutionScaleFactor::getScaleFactor(const JetParameters& parameters, Variation variation/* = Variation::NOMINAL*/) const {
        const JetResolutionObject::Record* record = m_object->getRecord(parameters);
        if (! record)
            return 1;

        const std::vector<float>& parameters_values = record->getParametersValues();
        return parameters_values[static_cast<size_t>(variation)];
    }

}
