#ifndef DEFINITIONS_H
#define DEFINITIONS_H


typedef float Realf;
typedef float Real;
typedef const float creal;

typedef std::array<size_t, 3> indices_t;


#define NBLOCKS 50
#define BLOCK_WID 4
#define PENCIL_WID NBLOCKS*BLOCK_WID 

typedef std::array<Real, BLOCK_WID> vblock_t;



#endif
