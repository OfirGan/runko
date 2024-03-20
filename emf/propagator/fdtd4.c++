#include "fdtd4.h"

#include <cmath>
#include "../../tools/iter/iter.h"

#ifdef GPU
#include <nvtx3/nvToolsExt.h> 
#endif

/*! \brief Update E field with full step
 *
 * Contains a dimension switch for solvers depending on internal mesh dimensions
 */


/// 2D E pusher
template<>
void fields::FDTD4<2>::push_e(fields::Tile<2>& tile)
{
#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();

  const float_m C1 = coeff1*corr*tile.cfl;
  const float_m C2 = coeff2*corr*tile.cfl;
  const int k = 0;

  UniIter::iterate2D(
  [=] DEVCALLABLE (int i, int j, YeeLattice &mesh)
  {
	mesh.ex(i,j,k) +=   C1*(-mesh.bz(i,  j-1,k) + mesh.bz(i,  j,  k)) 
		               +C2*(-mesh.bz(i,  j-2,k) + mesh.bz(i,  j+1,k));

	mesh.ey(i,j,k) +=   C1*( mesh.bz(i-1,j,  k) - mesh.bz(i,  j,  k)) 
		               +C2*( mesh.bz(i-2,j,  k) - mesh.bz(i+1,j,  k));

	mesh.ez(i,j,k) +=   C1*( mesh.bx(i,  j-1,k) - mesh.bx(i,  j,  k) 
                           - mesh.by(i-1,j,  k) + mesh.by(i,  j,  k))
                       +C2*( mesh.bx(i,  j-2,k) - mesh.bx(i,  j+1,k)
                            -mesh.by(i-2,j,  k) + mesh.by(i+1,j,  k));
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
void fields::FDTD4<3>::push_e(fields::Tile<3>& tile)
{

#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C1 = coeff1*corr*tile.cfl;
  const float_m C2 = coeff2*corr*tile.cfl;

  UniIter::iterate3D(
  [=] DEVCALLABLE (int i, int j, int k, YeeLattice &mesh)
  {
    mesh.ex(i,j,k)+= C1*(mesh.by(i,  j,  k-1) - mesh.by(i,  j,  k  )  
                       - mesh.bz(i,  j-1,k  ) + mesh.bz(i,  j,  k  ))
                    +C2*(mesh.by(i,  j,  k-2) - mesh.by(i,  j,  k+1)
                       - mesh.bz(i,  j-2,k  ) + mesh.bz(i,  j+1,k  ));

    mesh.ey(i,j,k)+= C1*(mesh.bz(i-1,j,  k  ) - mesh.bz(i,  j,  k  )  
                       - mesh.bx(i,  j,  k-1) + mesh.bx(i,  j,  k  ))
                    +C2*(mesh.bz(i-2,j,  k  ) - mesh.bz(i+1,j,  k  )
                       - mesh.bx(i,  j,  k-2) + mesh.bx(i,  j,  k+1));

    mesh.ez(i,j,k)+= C1*(mesh.bx(i,  j-1,k  ) - mesh.bx(i,  j,  k  )  
                       - mesh.by(i-1,j,  k  ) + mesh.by(i,  j,  k  ))
                    +C2*(mesh.bx(i,  j-2,k  ) - mesh.bx(i,  j+1,k  )
                       - mesh.by(i-2,j,  k  ) + mesh.by(i+1,j,  k  ));
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

/// 2D B pusher
template<>
void fields::FDTD4<2>::push_half_b(fields::Tile<2>& tile)
{
#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C1 = 0.5*coeff1*corr*tile.cfl;
  const float_m C2 = 0.5*coeff2*corr*tile.cfl;
  const int k = 0;

  UniIter::iterate2D(
  [=] DEVCALLABLE (int i, int j, YeeLattice &mesh)
  {
     mesh.bx(i,j,k) += C1*(-mesh.ez(i,  j+1,k) + mesh.ez(i,  j,  k))
	                  +C2*(-mesh.ez(i,  j+2,k) + mesh.ez(i,  j-1,k));
	 mesh.by(i,j,k) += C1*( mesh.ez(i+1,j,  k) - mesh.ez(i,  j,  k))
	                  +C2*( mesh.ez(i+2,j,  k) - mesh.ez(i-1,j,  k));
	 mesh.bz(i,j,k) += C1*( mesh.ex(i,  j+1,k) - mesh.ex(i,  j,  k)  
                           -mesh.ey(i+1,j,  k) + mesh.ey(i,  j,  k))
                      +C2*( mesh.ex(i,  j+2,k) - mesh.ex(i,  j-1,k)
                           -mesh.ey(i+2,j,  k) + mesh.ey(i-1,j,  k));

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
void fields::FDTD4<3>::push_half_b(fields::Tile<3>& tile)
{

#ifdef GPU
  nvtxRangePush(__PRETTY_FUNCTION__);
#endif

  YeeLattice& mesh = tile.get_yee();
  const float_m C1 = 0.5*coeff1*corr*tile.cfl;
  const float_m C2 = 0.5*coeff2*corr*tile.cfl;

  UniIter::iterate3D(
  [=] DEVCALLABLE (int i, int j, int k, YeeLattice &mesh)
  {
    mesh.bx(i,j,k)+= C1*(mesh.ey(i,  j,  k+1) - mesh.ey(i,  j,  k  )  
                       - mesh.ez(i,  j+1,k  ) + mesh.ez(i,  j,  k  ))
                    +C2*(mesh.ey(i,  j,  k+2) - mesh.ey(i,  j,  k-1)
                       - mesh.ez(i,  j+2,k  ) + mesh.ez(i,  j-1,k  ));

	mesh.by(i,j,k)+= C1*(mesh.ez(i+1,j,  k  ) - mesh.ez(i,  j,  k  )  
                       - mesh.ex(i,  j,  k+1) + mesh.ex(i,  j,  k  ))
                    +C2*(mesh.ez(i+2,j,  k  ) - mesh.ez(i-1,j,  k  )
                        -mesh.ex(i,  j,  k+2) + mesh.ex(i,  j,  k-1));

	mesh.bz(i,j,k)+= C1*(mesh.ex(i,  j+1,k  ) - mesh.ex(i,  j,  k  )  
                       - mesh.ey(i+1,j,  k  ) + mesh.ey(i,  j,  k  ))
                    +C2*(mesh.ex(i,  j+2,k  ) - mesh.ex(i,  j-1,k  )
                       - mesh.ey(i+2,j,  k  ) + mesh.ey(i-1,j,  k  ));

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



//template class fields::FDTD4<1>;
template class fields::FDTD4<2>;
template class fields::FDTD4<3>;
  
