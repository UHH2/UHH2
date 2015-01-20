#include "UHH2/common/include/TriggerSelection.h"

using namespace uhh2;
using namespace std;


TriggerSelection::TriggerSelection(string triggerName_):triggerName(triggerName_){
  triggerInit = false;
}

bool TriggerSelection::passes(const Event & event){
  if(!triggerInit){
    triggerIndex = event.get_trigger_index(triggerName);
    triggerInit = true;
  }

  return event.passes_trigger(triggerIndex);

}

