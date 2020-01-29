# -*- coding: utf-8 -*- 

# system libraries
from __future__ import print_function
from mpi4py import MPI
import numpy as np
import sys, os
import h5py

# runko + auxiliary modules
import pycorgi
import pyrunko
import pytools

# problem specific modules
from problem import Configuration_Problem as Configuration


# global simulation seed
np.random.seed(1)


def velocity_profile(xloc, ispcs, conf):

    #electrons
    if ispcs == 0:
        delgam  = conf.delgam #* np.abs(conf.mi / conf.me) * conf.temp_ratio

    #positrons/ions/second species
    elif ispcs == 1:
        delgam  = conf.delgam


    # perturb position between x0 + RUnif[0,1)
    # NOTE: Default injector changes odd ispcs's loc to (ispcs-1) prtcl loc
    #       This means that positrons/ions are on top of electrons to guarantee 
    #       charge conservation (because zero charge density initially)
    xx = xloc[0] + np.random.rand(1)
    yy = xloc[1] + np.random.rand(1)
    zz = xloc[2] + np.random.rand(1)


    gamma = conf.gamma
    direction = -1
    #ux, uy, uz, uu = boosted_maxwellian(delgam, gamma, direction=direction, dims=3)
    ux, uy, uz, uu = 0., 0., 0., 0.

    x0 = [xx, yy, zz]
    u0 = [ux, uy, uz]
    return x0, u0


def density_profile(xloc, ispcs, conf):
    # uniform plasma with default n_0 number density
    return conf.ppc


# Field initialization 
def insert_em_fields(grid, conf):

    #into radians
    btheta = conf.btheta/180.*np.pi
    bphi   = conf.bphi/180.*np.pi
    beta   = conf.beta

    for cid in grid.get_tile_ids():
        tile = grid.get_tile(cid)
        yee = tile.get_yee(0)

        ii,jj,kk = tile.index

        for n in range(conf.NzMesh):
            for m in range(-3, conf.NyMesh+3):
                for l in range(-3, conf.NxMesh+3):
                    # get global coordinates
                    iglob, jglob, kglob = pytools.pic.threeD.ind2loc( (ii,jj,kk), (l,m,n), conf)

                    yee.bx[l,m,n] = conf.binit*np.cos(btheta) 
                    yee.by[l,m,n] = conf.binit*np.sin(btheta)*np.sin(bphi)
                    yee.bz[l,m,n] = conf.binit*np.sin(btheta)*np.cos(bphi)   

                    yee.ex[l,m,n] = 0.0
                    yee.ey[l,m,n] =-beta*yee.bz[l,m,n]
                    yee.ez[l,m,n] = beta*yee.by[l,m,n]
    return


if __name__ == "__main__":

    # --------------------------------------------------
    # initial setup
    do_print = False
    if MPI.COMM_WORLD.Get_rank() == 0:
        do_print = True

    if do_print:
        print("Running pic.py with {} MPI processes.".format(MPI.COMM_WORLD.Get_size()))

    # --------------------------------------------------
    # Timer for profiling
    timer = pytools.Timer()

    timer.start("total")
    timer.start("init")
    timer.do_print = do_print

    # --------------------------------------------------
    # parse command line arguments
    args = pytools.parse_args()

    # create conf object with simulation parameters based on them
    conf = Configuration(args.conf_filename, do_print=do_print)


    # --------------------------------------------------
    # setup grid
    grid = pycorgi.threeD.Grid(conf.Nx, conf.Ny, conf.Nz)
    grid.set_grid_lims(conf.xmin, conf.xmax, conf.ymin, conf.ymax, conf.zmin, conf.zmax)

    # compute initial mpi ranks using Hilbert's curve partitioning
    pytools.balance_mpi_3D(grid)

    # load pic tiles into grid
    pytools.pic.threeD.load_tiles(grid, conf)

    # --------------------------------------------------
    # simulation restart

    # create output folders
    if grid.master():
        pytools.create_output_folders(conf)

    # get current restart file status
    io_stat = pytools.check_for_restart(conf)

    # no restart file; initialize simulation
    if io_stat["do_initialization"]:
        if do_print:
            print("initializing simulation...")
        lap = 0

        np.random.seed(1) #sync rnd generator seed for different mpi ranks 

        # injecting plasma particles
        prtcl_stat = pytools.pic.threeD.inject(grid, velocity_profile, density_profile, conf)  
        if do_print:
            print("injected:")
            print("    e- prtcls: {}".format(prtcl_stat[0]))
            print("    e+ prtcls: {}".format(prtcl_stat[1]))

        # inserting em grid
        insert_em_fields(grid, conf)

    else:
        if do_print:
            print("restarting simulation from lap {}...".format(io_stat["lap"]))

        pyrunko.fields.threeD.read_yee(grid, io_stat['read_lap'], io_stat['read_dir'])
        pyrunko.pic.threeD.read_prtcls(grid, io_stat['read_lap'], io_stat['read_dir'])

        lap = io_stat["lap"] + 1  # step one step ahead





    timer.stop("total")
    timer.stats("total")
