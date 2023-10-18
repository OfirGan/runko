#pragma once

#include "interaction.h"


namespace qed {
  using std::string;
  using std::tuple;


class Compton :
  public Interaction
{
public:

  Compton(string t1, string t2) : 
    Interaction(t1, t2)
  {
    name = "compton";
    //cross_section = 1.0;  // maximum cross section 
                          // for head-on collisions its x2 (in units of sigma_T)
  }

  //maximum cross section 
  const float_p cross_section = 1.0;

  bool no_electron_update = false; // do not update electrons
  bool no_photon_update   = false; // do not update photons

  double ming = 1.1;      // minimumjj electron energy to classify it "non-relativistic"
  double minx2z = 1.0e-2; // minimum ph energy needs to be > minx2z*gam 

  tuple<float_p, float_p> get_minmax_ene( string t1, string t2, double ene) override final;

  pair_float comp_cross_section(
    string t1, float_p ux1, float_p uy1, float_p uz1,
    string t2, float_p ux2, float_p uy2, float_p uz2) override;

  pair_float accumulate(string t1, float_p e1, string t2, float_p e2) override;

  void interact(
        string& t1, float_p& ux1, float_p& uy1, float_p& uz1,
        string& t2, float_p& ux2, float_p& uy2, float_p& uz2) override;


}; // end of Compton class


} // end of namespace qed

