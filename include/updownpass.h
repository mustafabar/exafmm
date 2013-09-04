#ifndef updownpass_h
#define updownpass_h
#include "kernel.h"
#include "logger.h"
#include "thread.h"

class UpDownPass : public Kernel, public Logger {
 public:
  real_t theta;                                                 //!< Multipole acceptance criteria

 private:
//! Error optimization of Rcrit
  void setRcrit(C_iter C, C_iter C0, real_t c) {
    spawn_tasks {                                               // Initialize tasks
      for (C_iter CC=C0+C->CHILD; CC!=C0+C->CHILD+C->NCHILD; CC++) {// Loop over child cells
	spawn_task0(setRcrit(CC, C0, c));                       //  Recursive call with new task
      }                                                         // End loop over child cells
      sync_tasks;                                               // Synchronize tasks
    }
#if Cartesian
    for (int i=1; i<NTERM; i++) C->M[i] /= C->M[0];             // Normalize multipole expansion coefficients
#endif
    real_t x = 1.0 / theta;                                     // Inverse of theta
#if ERROR_OPT
    assert(theta != 1.0);
    real_t a = c * powf(std::abs(C->M[0]),1.0/3);               // Cell coefficient
    for (int i=0; i<5; i++) {                                   // Newton-Rhapson iteration
      real_t f = x * x - 2 * x + 1 - a * std::pow(x,-P);        //  Function value
      real_t df = (P + 2) * x - 2 * (P + 1) + P / x;            //  Function derivative value
      x -= f / df;                                              //  Increment x
    }                                                           // End Newton-Rhapson iteration
#endif
    C->RCRIT *= x;                                              // Multiply Rcrit by error optimized parameter x
  }

//! Recursive call for upward pass
  void postOrderTraversal(C_iter C, C_iter C0) {
    spawn_tasks {                                               // Initialize tasks
      for (C_iter CC=C0+C->CHILD; CC!=C0+C->CHILD+C->NCHILD; CC++) {// Loop over child cells
	spawn_task0(postOrderTraversal(CC, C0));                //   Recursive call with new task
      }                                                         //  End loop over child cells
      sync_tasks;                                               //  Synchronize tasks
    }                                                           // Finalize tasks
    C->RMAX = 0;                                                // Initialzie Rmax
    C->M = 0;                                                   // Initialize multipole expansion coefficients
    C->L = 0;                                                   // Initialize local expansion coefficients
    P2M(C);                                                     // P2M kernel
    M2M(C,C0);                                                  // M2M kernel
  }

//! Recursive call for downward pass
  void preOrderTraversal(C_iter C, C_iter C0) const {
    L2L(C,C0);                                                  // L2L kernel
    L2P(C);                                                     // L2P kernel
    spawn_tasks {                                               // Initialize tasks
      for (C_iter CC=C0+C->CHILD; CC!=C0+C->CHILD+C->NCHILD; CC++) {// Loop over child cells
	spawn_task0(preOrderTraversal(CC, C0));                 //   Recursive call with new task
      }                                                         //  End loop over chlid cells
      sync_tasks;                                               //  Synchronize tasks
    }                                                           // Finalize tasks
  }

 public:
  UpDownPass(real_t _theta) : theta(_theta) {}

//! Upward pass (P2M, M2M)
  void upwardPass(Cells &cells) {
    startTimer("Upward pass");                                  // Start timer
    if (!cells.empty()) {                                       // If cell vector is not empty
      C_iter C0 = cells.begin();                                //  Set iterator of target root cell
      postOrderTraversal(C0, C0);                               //  Recursive call for upward pass
      real_t c = (1 - theta) * (1 - theta) / std::pow(theta,P+2) / powf(std::abs(C0->M[0]),1.0/3); // Root coefficient
      setRcrit(C0, C0, c);                                      //  Error optimization of Rcrit
      if( cells.size() > 9 ) {                                  //  If tree has more than 2 levels
        for (C_iter C=C0; C!=C0+9; C++) {                       //   Loop over top 2 levels of cells
          C->RCRIT = 1e12;                                      //    Prevent approximation
        }                                                       //   End loop over top 2 levels of cells
      }                                                         //  End if for tree levels
    }                                                           // End if for empty cell vector
    stopTimer("Upward pass");                                   // Stop timer
  }

//! Downward pass (L2L, L2P)
  void downwardPass(Cells &cells) {
    startTimer("Downward pass");                                // Start timer
    if (!cells.empty()) {                                       // If cell vector is not empty
      C_iter C0 = cells.begin();                                //  Root cell
      L2P(C0);                                                  //  If root is the only cell do L2P
      spawn_tasks {                                             //  Initialize tasks
        for (C_iter CC=C0+C0->CHILD; CC!=C0+C0->CHILD+C0->NCHILD; CC++) {// Loop over child cells
          spawn_task0(preOrderTraversal(CC, C0));               //    Recursive call for downward pass
        }                                                       //   End loop over child cells
        sync_tasks;                                             //   Synchronize tasks
      }                                                         //  Finalize tasks
    }                                                           // End if for empty cell vector
    stopTimer("Downward pass");                                 // Stop timer
  }

//! Get dipole of entire system 
  vec3 getDipole(Bodies &bodies, vec3 X0) {
    vec3 dipole = 0;                                            // Initialize dipole correction
    for (B_iter B=bodies.begin(); B!=bodies.end(); B++) {       // Loop over bodies
      dipole += (B->X - X0) * B->SRC;                           //  Calcuate dipole of the whole system
    }                                                           // End loop over bodies
    return dipole;                                              // Return dipole
  }

//! Dipole correction
  void dipoleCorrection(Bodies &bodies, vec3 dipole, int numBodies, real_t cycle) {
    real_t coef = 4 * M_PI / (3 * cycle * cycle * cycle);       // Precalcualte constant
    for (B_iter B=bodies.begin(); B!=bodies.end(); B++) {       // Loop over bodies
      B->TRG[0] -= coef * norm(dipole) / numBodies / B->SRC;    //  Dipole correction for potential
      for (int d=0; d!=3; d++) {                                //  Loop over dimensions
        B->TRG[d+1] -= coef * dipole[d];                        //   Dipole correction for forces
      }                                                         //  End loop over dimensions
    }                                                           // End loop over bodies
  }
};
#endif