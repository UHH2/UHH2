#ifndef JetResolution_h
#define JetResolution_h

// In standalone mode, no reference to CMSSW exists, so the only way to retrieve resolutions and scale factors are from text files.

#include <UHH2/JetMETObjects/interface/JetResolutionObject.h>

namespace JME {
    class JetResolution {
        public:
            JetResolution(const std::string& filename);
            JetResolution(const JetResolutionObject& object);
            JetResolution() {
                // Empty
            }

            float getResolution(const JetParameters& parameters) const;

            void dump() const {
                m_object->dump();
            }

            // Advanced usage
            const JetResolutionObject* getResolutionObject() const {
                return m_object.get();
            }

        private:
            std::shared_ptr<JetResolutionObject> m_object;
    };

    class JetResolutionScaleFactor {
        public:
            JetResolutionScaleFactor(const std::string& filename);
            JetResolutionScaleFactor(const JetResolutionObject& object);
            JetResolutionScaleFactor() {
                // Empty
            }

            float getScaleFactor(const JetParameters& parameters, Variation variation = Variation::NOMINAL) const;

            void dump() const {
                m_object->dump();
            }

            // Advanced usage
            const JetResolutionObject* getResolutionObject() const {
                return m_object.get();
            }

        private:
            std::shared_ptr<JetResolutionObject> m_object;
    };

};

#endif

