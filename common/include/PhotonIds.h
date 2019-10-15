#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

// see also ElectronIds.h for general comments

/* class PhotonID{ */
/*  public: */
/*   explicit PhotonID(Photon::Selector sel_); */

/*   bool operator()(const Photon & pho, const uhh2::Event & event) const; */

/*  private: */
/*   Photon::Selector sel; */

/* }; */

// Generic selector for the stored Photon tags

class PhotonTagID{
 public:
  PhotonTagID(Photon::tag sel_): sel(sel_) {}

  bool operator()(const Photon & pho, const uhh2::Event &) const {
    return pho.get_tag(sel); // no need to check, done in Tag class
  }

 private:
  Photon::tag sel;

};
