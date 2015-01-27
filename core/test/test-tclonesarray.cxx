#include "TClonesArray.h"

// Reproduce ROOT bug, which segfaults on calling
// EpandCreateFast on small TClonesArray.

int main(){
    TClonesArray * ar = new TClonesArray();
    ar->SetClass("TObject", 1);
    ar->ExpandCreateFast(11);
}

