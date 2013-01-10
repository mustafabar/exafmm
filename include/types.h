#ifndef types_h
#define types_h
#include <complex>
#include "macros.h"
#include <queue>
#include <utility>
#include <vector>
#include "vec.h"

#if defined(REAL_TYPE) 
#define REAL_TYPE_FLOAT 0
#define REAL_TYPE_DOUBLE 1
#endif

#if defined(KAHAN) 
#define KAHAN_NEVER 0 		/* never use Kahan's accumulation */
#define KAHAN_IN_DIRECT 1	/* use it only in the direct evaluation */
#define KAHAN_ALWAYS 2 /* use it both in main computation and direct evaluation */
#endif


// Basic type definitions
#if defined(REAL_TYPE) && REAL_TYPE == REAL_TYPE_DOUBLE
typedef double               real_t;                            //!< Floating point type
#else
typedef float                real_t;                            //!< Floating point type
#endif
typedef std::complex<real_t> complex_t;                         //!< Complex type
typedef vec<3,real_t>        vec3;                              //!< Vector of 3 floating point types
typedef vec<3,float>         fvec3;                             //!< Vector of 3 single precision types
typedef vec<4,real_t>        vec4;                              //!< Vector of 4 floating point types
typedef vec<8,int>           ivec8;                             //!< Vector of 8 integer types
typedef std::pair<vec3,vec3> vec3Pair;                          //!< Pair of vec3

// Compile-time parameters
#ifdef MULTIPOLE_EXPANSION_ORDER
const int P = MULTIPOLE_EXPANSION_ORDER;                        //!< Order of expansions
#else
const int P = 3;                                                //!< Order of expansions
#endif
const float EPS2 = .0;                                          //!< Softening parameter (squared)
#if COMkernel
const int MTERM = P*(P+1)*(P+2)/6-3;                            //!< Number of Cartesian mutlipole terms
#else
const int MTERM = P*(P+1)*(P+2)/6;                              //!< Number of Cartesian mutlipole terms
#endif
const int LTERM = (P+1)*(P+2)*(P+3)/6;                          //!< Number of Cartesian local terms
const int NTERM = P*(P+1)/2;                                    //!< Number of Spherical multipole/local terms

#if Cartesian
typedef vec<MTERM,real_t> vecM;                                 //!< Multipole coefficient type for Cartesian
typedef vec<LTERM,real_t> vecL;                                 //!< Local coefficient type for Cartesian
#elif Spherical
typedef vec<NTERM,complex_t> vecM;                              //!< Multipole coefficient type for spherical
typedef vec<NTERM,complex_t> vecL;                              //!< Local coefficient type for spherical
#endif

//! Structure of aligned source for SIMD
struct Source {
  vec3   X;                                                     //!< Position
  real_t SRC;                                                   //!< Scalar source values
} __attribute__ ((aligned (16)));

//! Structure of bodies
struct Body : public Source {
  int    IBODY;                                                 //!< Initial body numbering for sorting back
  int    IPROC;                                                 //!< Initial process numbering for partitioning back
  int    ICELL;                                                 //!< Cell index
  vec4   TRG;                                                   //!< Scalar+vector3 target values
#if KAHAN >= KAHAN_IN_DIRECT
  vec4   TRGc;                                                   //!< Scalar+vector3 target values
#endif
};
typedef std::vector<Body>           Bodies;                     //!< Vector of bodies
typedef std::vector<Body>::iterator B_iter;                     //!< Iterator of body vector

//! Structure of cells
struct Cell {
  int       NCHILD;                                             //!< Number of child cells
  int       NCBODY;                                             //!< Number of child bodies
  int       NDBODY;                                             //!< Number of descendant bodies
  int       PARENT;                                             //!< Index of parent cell
  int       CHILD;                                              //!< Index of child cells
  long long ICELL;                                              //!< Cell index
  B_iter    BODY;                                               //!< Iterator of first body
  vec3      X;                                                  //!< Cell center
  real_t    R;                                                  //!< Cell radius
  real_t    RMAX;                                               //!< Max cell radius
  real_t    RCRIT;                                              //!< Critical cell radius
  vecM      M;                                                  //!< Multipole coefficients
  vecL      L;                                                  //!< Local coefficients
};
typedef std::vector<Cell>           Cells;                      //!< Vector of cells
typedef std::vector<Cell>::iterator C_iter;                     //!< Iterator of cell vector
typedef std::queue<C_iter>          CellQueue;                  //!< Queue of cell iterators

#endif
