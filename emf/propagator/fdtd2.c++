#include "fdtd2.h"
#include <cmath>
#include "../../tools/iter/iter.h"

#ifdef GPU
#include <nvtx3/nvToolsExt.h> 
#endif


/*! \brief Update E field with full step
 *
 * Contains a dimension switch for solvers depending on internal mesh dimensions
 */


/// 1D E pusher
template<>
void fields::FDTD2<1>::push_e(fields::Tile<1>& tile)
{
#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C = 1.0 * tile.cfl * dt * corr;

  UniIter::iterate(
  [=] DEVCALLABLE (int i, YeeLattice &mesh)
  {
    // Ex NONE
    mesh.ey(i,0,0) += + C*( mesh.bz(i-1,0,0) - mesh.bz(i,0,0));
    mesh.ez(i,0,0) += + C*(-mesh.by(i-1,0,0) + mesh.by(i,0,0));

  }, 
    tile.mesh_lengths[0], 
    mesh);

  UniIter::sync();

#ifdef GPU
  nvtxRangePop();
#endif

}

/// 2D E pusher
template<>
void fields::FDTD2<2>::push_e(fields::Tile<2>& tile)
{
#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C = 1.0 * tile.cfl * dt * corr;

  UniIter::iterate2D(
  [=] DEVCALLABLE (int i, int j, YeeLattice &mesh)
  {

    mesh.ex(i,j,0) += + C*(-mesh.bz(i,  j-1,0) + mesh.bz(i,j,0));
    mesh.ey(i,j,0) += + C*( mesh.bz(i-1,j,  0) - mesh.bz(i,j,0));
    mesh.ez(i,j,0) += + C*( mesh.bx(i,  j-1,0) - mesh.bx(i,j,0) 
                           -mesh.by(i-1,j,  0) + mesh.by(i,j,0));

  }, 
    tile.mesh_lengths[0], 
    tile.mesh_lengths[1], 
    mesh);

  UniIter::sync();

#ifdef GPU
  nvtxRangePop();
#endif
}


/// 3D E pusher
template<>
void fields::FDTD2<3>::push_e(fields::Tile<3>& tile)
{

#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C = 1.0 * tile.cfl * dt * corr;

  UniIter::iterate3D(
  [=] DEVCALLABLE (int i, int j, int k, YeeLattice &mesh)
  {
    mesh.ex(i,j,k) += + C*( mesh.by(i,  j,  k-1) - mesh.by(i,j,k))
                      + C*(-mesh.bz(i,  j-1,k  ) + mesh.bz(i,j,k)); 
    mesh.ey(i,j,k) += + C*( mesh.bz(i-1,j,  k  ) - mesh.bz(i,j,k))
                      + C*(-mesh.bx(i,  j,  k-1) + mesh.bx(i,j,k));
    mesh.ez(i,j,k) += + C*( mesh.bx(i,  j-1,k  ) - mesh.bx(i,j,k))
                      + C*(-mesh.by(i-1,j,  k  ) + mesh.by(i,j,k));
  }, 
    tile.mesh_lengths[0], 
    tile.mesh_lengths[1], 
    tile.mesh_lengths[2], 
    mesh);

  UniIter::sync();

#ifdef GPU
  nvtxRangePop();
#endif
}


//--------------------------------------------------

/// Update B field with a half step

/// 1D B pusher
template<>
void fields::FDTD2<1>::push_half_b(fields::Tile<1>& tile)
{
#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C = 0.5 * tile.cfl * dt * corr;

  UniIter::iterate(
  [=] DEVCALLABLE (int i, YeeLattice &mesh)
  {
    // Bx NONE
    mesh.by(i,0,0) += + C*( mesh.ez(i+1,0,0) - mesh.ez(i,0,0));
    mesh.bz(i,0,0) += + C*(-mesh.ey(i+1,0,0) + mesh.ey(i,0,0));
  }, 
    tile.mesh_lengths[0], 
    mesh);

  UniIter::sync();

#ifdef GPU
  nvtxRangePop();
#endif

}

/// 2D B pusher
template<>
void fields::FDTD2<2>::push_half_b(fields::Tile<2>& tile)
{
#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C = 0.5 * tile.cfl * dt * corr;

  UniIter::iterate2D(
  [=] DEVCALLABLE (int i, int j, YeeLattice &mesh)
  {
    mesh.bx(i,j,0) += + C*(-mesh.ez(i,  j+1,0) + mesh.ez(i,j,0));
    mesh.by(i,j,0) += + C*( mesh.ez(i+1,j,  0) - mesh.ez(i,j,0));
    mesh.bz(i,j,0) += + C*( mesh.ex(i,  j+1,0) - mesh.ex(i,j,0)
                           -mesh.ey(i+1,j,  0) + mesh.ey(i,j,0));

  }, 
    tile.mesh_lengths[0], 
    tile.mesh_lengths[1], 
    mesh);

  UniIter::sync();

#ifdef GPU
  nvtxRangePop();
#endif
}


/// 3D B pusher
template<>
void fields::FDTD2<3>::push_half_b(fields::Tile<3>& tile)
{

#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C = 0.5 * tile.cfl * dt * corr;

  UniIter::iterate3D(
  [=] DEVCALLABLE (int i, int j, int k, YeeLattice &mesh)
  {
    mesh.bx(i,j,k) += + C*( mesh.ey(i,  j,  k+1) - mesh.ey(i,j,k))
                      + C*(-mesh.ez(i,  j+1,k  ) + mesh.ez(i,j,k));
    mesh.by(i,j,k) += + C*( mesh.ez(i+1,j,  k  ) - mesh.ez(i,j,k))
                      + C*(-mesh.ex(i,  j,  k+1) + mesh.ex(i,j,k));
    mesh.bz(i,j,k) += + C*( mesh.ex(i,  j+1,k  ) - mesh.ex(i,j,k))
                      + C*(-mesh.ey(i+1,j,  k  ) + mesh.ey(i,j,k));
  }, 
    tile.mesh_lengths[0], 
    tile.mesh_lengths[1], 
    tile.mesh_lengths[2], 
    mesh);

  UniIter::sync();

#ifdef GPU
  nvtxRangePop();
#endif
}



template class fields::FDTD2<1>;
template class fields::FDTD2<2>;
template class fields::FDTD2<3>;
