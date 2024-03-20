#include "compensator.h"

#include <cmath>
#include "../../tools/iter/devcall.h"
#include "../../tools/iter/iter.h"
#include "../../tools/iter/allocator.h"


#ifdef GPU
#include <nvtx3/nvToolsExt.h> 
#endif


/// 2D 3-point compensator filter from Birdsall & Langdon
//
// Filter is (20,-1,-1) with normalization 1/12
//
//  NOTE: Main difference to a binomial compensator are
//  the suppressed corners
//
template<>
void fields::Compensator2<2>::solve(
    fields::Tile<2>& tile)
{
  // 2D general coefficients
  const double winv=1./12.; //normalization
  const double wtm=20.0*winv, //middle M
               wts=-1.0*winv, //side   S
               wtc=-1.0*winv; //corner K


  auto& mesh = tile.get_yee();
  const int H = 2; 
  const int k = 0;

  // make 2d loop with shared memory 
  auto fun = 
  [=] DEVCALLABLE (int i, int j, 
                   toolbox::Mesh<float_m, 3> &jj, 
                   toolbox::Mesh<float_m, 3> &tmp)
  {
    tmp(i-H,j-H,k) = 
      mesh.jx(i-1-H, j-1-H, k)*wtc + 
      mesh.jx(i  -H, j-1-H, k)*wts + 
      mesh.jx(i+1-H, j-1-H, k)*wtc + 
      mesh.jx(i-1-H, j  -H, k)*wts + 
      mesh.jx(i  -H, j  -H, k)*wtm + 
      mesh.jx(i+1-H, j  -H, k)*wts + 
      mesh.jx(i-1-H, j+1-H, k)*wtc + 
      mesh.jx(i  -H, j+1-H, k)*wts + 
      mesh.jx(i+1-H, j+1-H, k)*wtc;
  };
    
  //--------------------------------------------------
  // Jx
  tmp.clear();
  UniIter::iterate2D(fun, 
        tile.mesh_lengths[0] + 2*H, 
        tile.mesh_lengths[1] + 2*H,
        mesh.jx, tmp);
 
  UniIter::sync();
  std::swap(mesh.jx, tmp);

  //--------------------------------------------------
  // Jy
  tmp.clear();
  UniIter::iterate2D(fun, 
        tile.mesh_lengths[0] + 2*H, 
        tile.mesh_lengths[1] + 2*H,
        mesh.jy, tmp);
 
  UniIter::sync();
  std::swap(mesh.jy, tmp);

  //--------------------------------------------------
  // Jz
  tmp.clear();
  UniIter::iterate2D(fun, 
        tile.mesh_lengths[0] + 2*H, 
        tile.mesh_lengths[1] + 2*H,
        mesh.jz, tmp);
 
  UniIter::sync();
  std::swap(mesh.jz, tmp);

  //--------------------------------------------------
#ifdef GPU
  nvtxRangePop();
#endif
}



//template class fields::Compensator2<1>; // 1D
  template class fields::Compensator2<2>; // 2D
//template class fields::Compensator2<3>; // 3D
