include ../Makefile.include
SERIALRUN = ./a.out
#PARALLELRUN = mpirun -np 3 -machinefile ~/hosts ./a.out
PARALLELRUN = mpirun -np 4 ./a.out

topdown: construct.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS) -DTOPDOWN
	$(SERIALRUN)

bottomup: construct.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(SERIALRUN)

kernel: kernel.cxx $(SOURCE)
	for P in 3 4 5 6 7 8 9 10 11 12 13 14 15; do echo P = $$P compiling && \
        $(CXX) $? $(LFLAGS) -DEXPANSION=$$P && \
        echo P = $$P executing && $(SERIALRUN); done

ewald_direct: ewald_direct.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)
	$(SERIALRUN)

ewald_fmm: ewald_fmm.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)
	$(SERIALRUN)

serialrun: serialrun.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)

unsort: unsort.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(SERIALRUN)

ijserialrun: ijserialrun.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(SERIALRUN)

Nserial: Nserial.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)

direct_gpu: direct_gpu.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)
	$(SERIALRUN)

mpi: mpi.cxx
	$(CXX) $? $(LFLAGS) -pg
	$(PARALLELRUN)
	gprof -b a.out gmon.out

check_gpus: check_gpus.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)
	$(PARALLELRUN)

shift: shift.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

nth_element: nth_element.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

bisection: bisection.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

let: let.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

parallelrun: parallelrun.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

unpartition: unpartition.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

ijparallelrun: ijparallelrun.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

Nparallel: Nparallel.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)
	$(PARALLELRUN)

skip_tree: skip_tree.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

overlap_comm: overlap_comm.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(PARALLELRUN)

vtk: vtk.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(SERIALRUN)

fdgl: fdgl.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS) $(VFLAGS)
	$(SERIALRUN)

poisson: poisson.cxx $(OBJECT)
	$(CXX) $? $(LFLAGS)
	$(SERIALRUN)

all      :
	make topdown
	make bottomup
	make kernel
	make serialrun
	make unsort
	make ijserialrun
	make direct_gpu
	make mpi
	make check_gpus
	make shift
	make nth_element
	make bisection
	make let
	make parallelrun
	make unpartition
	make ijparallelrun
	make skip_tree
	make overlap_comm
