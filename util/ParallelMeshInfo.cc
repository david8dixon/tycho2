/*
Copyright (c) 2016, Los Alamos National Security, LLC
All rights reserved.

Copyright 2016. Los Alamos National Security, LLC. This software was produced 
under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National 
Laboratory (LANL), which is operated by Los Alamos National Security, LLC for 
the U.S. Department of Energy. The U.S. Government has rights to use, 
reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR LOS 
ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR 
ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified 
to produce derivative works, such modified software should be clearly marked, 
so as not to confuse it with the version available from LANL.

Additionally, redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following conditions 
are met:
1.      Redistributions of source code must retain the above copyright notice, 
        this list of conditions and the following disclaimer.
2.      Redistributions in binary form must reproduce the above copyright 
        notice, this list of conditions and the following disclaimer in the 
        documentation and/or other materials provided with the distribution.
3.      Neither the name of Los Alamos National Security, LLC, Los Alamos 
        National Laboratory, LANL, the U.S. Government, nor the names of its 
        contributors may be used to endorse or promote products derived from 
        this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL 
SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string>
#include <cstdio>
#include <cassert>
#include <mpi.h>
#include "Comm.hh"
#include "ParallelMesh.hh"

using namespace std;


/*
    main
*/
int main(int argc, char* argv[])
{
    ParallelMesh parallelMesh;
    ParallelMesh::PartitionData partData;
    string inputFile;
    bool verbose = false;
    
    
    // Start MPI
    MPI_Init(&argc, &argv);


    // Print utility name
    if (Comm::rank() == 0) {
        printf("--- ParallelMeshInfo Utility ---\n");
    }
    

    // Get input file
    if (argc == 2) {
        verbose = false;
    }
    else if (argc == 3 && strcmp(argv[2], "verbose") == 0) {
        verbose = true;
    }
    else {
        if (Comm::rank() == 0) {
            printf("Incorrect number of arguments\n");
            printf("Usage: ./ParallelMeshInfo.x <inputFile> (verbose)\n");
            printf("\n\n\n");
        }
        return 0;
    }
    inputFile = argv[1];
    
    
    // Read in parallel mesh serially and print info
    if (Comm::rank() == 0) {
        printf("Parallel mesh data read serially.\n");
        parallelMesh.read(inputFile);
        parallelMesh.print(verbose);
    }
    Comm::barrier();


    // Read parallel mesh in parallel and print info
    if (Comm::rank() == 0) {
        printf("\n\n\nParallel mesh data read in parallel.\n");
    }
    ParallelMesh::readInParallel(inputFile, partData);
    for (int part = 0; part < Comm::numRanks(); part++) {
        if (Comm::rank() == part) {
            printf("Partition %d\n", Comm::rank());
            ParallelMesh::printPartitionData(partData, verbose);
        }
        Comm::barrier();
    }

    
    // Cleanup
    Comm::barrier();
    if (Comm::rank() == 0) {
        printf("\n\n\n");
    }
    MPI_Finalize();
    return 0;
}



