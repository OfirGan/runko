#pragma once

#include <array>
#include <mpi4cpp/mpi.h>

#include "../definitions.h"
#include "../corgi/tile.h"
#include "../corgi/corgi.h"
#include "../em-fields/tile.h"
#include "particle.h"


namespace pic {

using namespace mpi4cpp;

/*! \brief PiC tile
 *
 * Tile infrastructures are inherited from corgi::Tile
 * Maxwell field equation solver is inherited from fields::Tile
*/

template<std::size_t D>
class Tile :
  virtual public fields::Tile<D>, 
  virtual public  corgi::Tile<D> 
{


public:

  using corgi::Tile<D>::mins;
  using corgi::Tile<D>::maxs;
  using fields::Tile<D>::mesh_lengths;

  using fields::Tile<D>::yee;
  using fields::Tile<D>::analysis;

  using fields::Tile<D>::cfl;

  std::vector<ParticleContainer> containers;

  //--------------------------------------------------
  // normal container methods
     
  /// get i:th container
  ParticleContainer& get_container(int i) { return containers[i]; };

  const ParticleContainer& get_const_container(int i) const { return containers[i]; };

  /// set i:th container
  void set_container(ParticleContainer& block) 
  { 
    block.wrapper_dimension = D; // set wrapper dimension based on tile dimension
    containers.push_back(block); 
  };

  int Nspecies() const { return containers.size(); };


  /// constructor
  Tile(int nx, int ny, int nz) :
     corgi::Tile<D>(),
    fields::Tile<D>{nx,ny,nz}
  { }


  //--------------------------------------------------
  // MPI send
  virtual std::vector<mpi::request> 
  send_data( mpi::communicator&, int orig, int mode, int tag) override;

  /// actual tag=0 send
  std::vector<mpi::request> 
  send_particle_data( mpi::communicator&, int orig, int tag);

  /// actual tag=1 send
  std::vector<mpi::request> 
  send_particle_extra_data( mpi::communicator&, int orig, int tag);


  //--------------------------------------------------
  // MPI recv
  virtual std::vector<mpi::request> 
  recv_data(mpi::communicator&, int dest, int mode, int tag) override;

  /// actual tag=0 recv
  std::vector<mpi::request> 
  recv_particle_data(mpi::communicator&, int dest, int tag);

  /// actual tag=1 recv
  std::vector<mpi::request> 
  recv_particle_extra_data(mpi::communicator&, int dest, int tag);
  //--------------------------------------------------


  /// check all particle containers for particles
  // exceeding limits
  void check_outgoing_particles();

  /// delete particles from each container that are exceeding
  // the boundaries
  void delete_transferred_particles();

  /// get particles flowing into this tile
  void get_incoming_particles(corgi::Grid<D>& grid);

  /// pack all particles for MPI message
  void pack_all_particles();

  /// pack particles for MPI message
  void pack_outgoing_particles();

  /// unpack received MPI message particles
  void unpack_incoming_particles();

  /// delete all particles from each container
  void delete_all_particles();

  /// shrink to fit all internal containers
  void shrink_to_fit_all_particles();


private:
  std::size_t dim = D;
};



} // end of namespace pic
