import numpy as np
import os, sys
import copy
import matplotlib.pyplot as plt
import palettable as pal
from matplotlib import cm


from visualize import *

import vmesh



#physical "real" distribution to compare against
def physical_vel(x,y,z):

    mux = 2.0
    muy = 0.0
    muz = 0.0
    sigmax = 5.0
    sigmay = 5.0
    sigmaz = 5.0

    vx = np.exp(-(x-mux)**2 / sigmax**2 )
    vy = np.exp(-(y-muy)**2 / sigmay**2 )
    vz = np.exp(-(z-muz)**2 / sigmaz**2 )

    return vx*vy*vz


""""
# velocity block with data and pointers to neighbors
class vBlock:

    data = 0.0 # value of f(vx,vy,vz) at location (x,y,z)
    loc  = (0.0, 0.0, 0.0)  #block location
    dls  = (0.0, 0.0, 0.0)  #block dimensions

    refLevel = 0
    
    def __init__(self, vx,vy,vz, dx,dy,dz):

        self.loc = (vx, vy, vz)
        self.dls = (dx, dy, dz)

"""


"""
class vMesh:

    mesh = []


    def __init__(self, mins, maxs, dvs):


        mins += dvs/2.0
        maxs -= dvs/2.0

        zi = mins[2]
        while zi <= maxs[2]:

            yi = mins[1]
            while yi <= maxs[1]:

                xi = mins[0]
                while xi <= maxs[0]:
                    #print "({},{},{})".format(xi, yi, zi)

                    vb      = vBlock( xi, yi, zi , dvs[0], dvs[1], dvs[2] )
                    vb.data = physical_vel(xi, yi, zi)

                    self.mesh.append( vb )

                    xi += dvs[0]
                yi += dvs[1]
            zi += dvs[2]

"""


def cellID2index(cellID, dvs):

    cellID -= 1

    k = np.int(  ( cellID / (dvs[0] * dvs[1]) ) )
    j = np.int(  ( cellID / dvs[0] ) % dvs[1] )
    i = np.int(  cellID % dvs[0] )

    return (i,j,k)




if __name__ == "__main__":


    ################################################## 
    # set up plotting and figure
    plt.fig = plt.figure(1, figsize=(8,4))
    plt.rc('font', family='serif', size=12)
    plt.rc('xtick')
    plt.rc('ytick')
    
    gs = plt.GridSpec(1, 2)
    gs.update(hspace = 0.5)
    
    axs = []
    axs.append( plt.subplot(gs[0]) )
    axs.append( plt.subplot(gs[1]) )


    ################################################## 
    # set-up grid
    mins = [ -10.0, -10.0, -1.0 ]
    maxs = [  10.0,  10.0,  1.0 ]
    dvs  = [  2.0,    2.0,  2.0 ]

    mesh = vmesh.vMesh()
    mesh.zFill(mins, maxs, dvs)

    blocks = mesh.all_blocks(True)
    print "received {} blocks vs {} ".format(len(blocks), mesh.nBlocks)
    print "mesh dimensions: {}".format( mesh.nCells)

    print "Begin testing indices..."
    tests = [ [1,1,0], [2,2,0], [5,3,0] ]
    for test in tests:
        print "    testing for...", test
        cid = mesh.get_block_ID( test )
        print "    ", cid
        print "    ", cellID2index( cid , mesh.nCells )
        print "    ", mesh.get_indices( cid )

    print "next test mesh indexing..."
    print mesh[1]
    mesh[1] = [1.0, 2.0, 3.0, 4.5]
    print mesh[1]

    print mesh[2,2,0]
    mesh[2,2,0] = [3.0, 2.0, 1.0, 0.5]
    print mesh[2,2,0]



    visualize_mesh(axs[0], mesh)

    print "next clipping..."
    #mesh.clip()



    plt.savefig("vmesh.png")


