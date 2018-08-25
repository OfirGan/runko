

import pycorgi
import pyplasmabox 



#make random starting order
def loadMpiRandomly(n):
    np.random.seed(4)
    if n.master:
        for i in range(n.getNx()):
            for j in range(n.getNy()):
                val = np.random.randint(n.Nrank)
                n.setMpiGrid(i, j, val)


#load nodes to be in stripe formation (splitted in Y=vertical direction)
def loadMpiYStrides(n):
    if n.master: #only master initializes; then sends
        stride = np.zeros( (n.getNy()), np.int64)
        dy = np.float(n.getNy()) / np.float(n.Nrank) 
        for j in range(n.getNy()):
            val = np.int( j/dy )
            stride[j] = val

        for i in range(n.getNx()):
            for j in range(n.getNy()):
                val = stride[j]
                n.setMpiGrid(i, j, val)
    n.bcastMpiGrid()

#load nodes to be in stripe formation (splitted in X=horizontal direction)
def loadMpiXStrides(n):
    if n.master: #only master initializes; then sends
        stride = np.zeros( (n.getNx()), np.int64)
        dx = np.float(n.getNx()) / np.float(n.Nrank) 
        for i in range(n.getNx()):
            val = np.int( i/dx )
            stride[i] = val

        for j in range(n.getNy()):
            for i in range(n.getNx()):
                val = stride[i]
                n.setMpiGrid(i, j, val)
    n.bcastMpiGrid()


#load tiles into each node
def loadTiles(n, conf):
    for i in range(n.getNx()):
        for j in range(n.getNy()):
            #print("{} ({},{}) {} ?= {}".format(n.rank, i,j, n.getMpiGrid(i,j), ref[j,i]))

            if n.getMpiGrid(i,j) == n.rank:
                c = pyplasmabox.vlv.oneD.Tile(conf.NxMesh, conf.NyMesh, conf.NzMesh)

                #initialize tile dimensions 
                c.cfl = conf.cfl
                c.dt = conf.dt
                c.dx = conf.dx
                #c.dy = conf.dy
                #c.dz = conf.dz

                # initialize analysis tiles ready for incoming simulation data
                for ip in range(conf.Nspecies):
                    c.addAnalysisSpecies()

                #add it to the node
                n.addTile(c) 




#def initializeEmptyVelocityMesh(mesh, conf):
#    mesh.Nblocks = [conf.Nvx, conf.Nvy, conf.Nvz]
#
#    pmins = [conf.vxmin, conf.vymin, conf.vzmin]
#    pmaxs = [conf.vxmax, conf.vymax, conf.vzmax]
#    mesh.zFill( pmins, pmaxs )
#
#
#
## create empty vmesh object according to conf specifications
#def createEmptyVelocityMesh(conf):
#    mesh = ptools.VeloMesh()
#    initializeEmptyVelocityMesh(mesh, conf)
#
#    return mesh
#









