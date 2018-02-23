// @HEADER
// ***********************************************************************
//
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
// @HEADER
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <Tpetra_DefaultPlatform.hpp>
#include <Tpetra_Version.hpp>

#include <MatrixMarket_Tpetra.hpp>

#include <Teuchos_RCP.hpp>
#include <Teuchos_GlobalMPISession.hpp>
#include <Teuchos_FancyOStream.hpp>

#include "typedefs.hpp"
#include "utils.hpp"
#include "MeshDatabase.hpp"

//#define PRINT_VERBOSE 1

using namespace BlockCrsTest;

int main (int argc, char *argv[]) 
{
  using Teuchos::RCP;
  using Teuchos::TimeMonitor;

  const auto INVALID_GLOBAL_ORDINAL = Teuchos::OrdinalTraits<global_ordinal_type>::invalid();

  auto out = Teuchos::getFancyOStream (Teuchos::rcpFromRef (std::cout));
  
  // MPI boilerplate
  Teuchos::GlobalMPISession mpiSession (&argc, &argv, NULL);
  RCP<const Teuchos::Comm<int> > comm = Tpetra::DefaultPlatform::getDefaultPlatform ().getComm();

  // Command-line input
  LO num_global_elements_i = 2, num_global_elements_j = 2, num_global_elements_k = 2;
  LO num_procs_i = 1, num_procs_j = 1, num_procs_k = 1;
  LO blocksize = 5, nrhs = 1, repeat = 100;

  Teuchos::CommandLineProcessor clp(false);
  clp.setDocString("BlockCrs performance test using 3D 7 point stencil.\n");
  clp.setOption("num-elements-i", &num_global_elements_i, "Number of cells in the I dimension.");
  clp.setOption("num-elements-j", &num_global_elements_j, "Number of cells in the J dimension.");
  clp.setOption("num-elements-k", &num_global_elements_k, "Number of cells in the K dimension.");
  clp.setOption("num-procs-i", &num_procs_i, "Processor grid of (npi,npj,npk); npi*npj*npk should be equal to the number of MPI ranks.");
  clp.setOption("num-procs-j", &num_procs_j, "Processor grid of (npi,npj,npk); npi*npj*npk should be equal to the number of MPI ranks.");
  clp.setOption("num-procs-k", &num_procs_k, "Processor grid of (npi,npj,npk); npi*npj*npk should be equal to the number of MPI ranks.");
  clp.setOption("blocksize", &blocksize, "Block size. The # of DOFs coupled in a multiphysics flow problem.");
  clp.setOption("nrhs", &nrhs, "Number of right hand sides to solve for.");
  clp.setOption("repeat", &repeat, "Number of iterations of matvec operations to measure performance.");

  {
    auto r_parse = clp.parse(argc, argv, comm->getRank() == 0 ? &std::cout : NULL);
    if (r_parse == Teuchos::CommandLineProcessor::PARSE_HELP_PRINTED) return 0;
    if (r_parse != Teuchos::CommandLineProcessor::PARSE_SUCCESSFUL  ) return -1;

    TEUCHOS_ASSERT(num_procs_i*num_procs_j*num_procs_k == comm->getSize());
  }
  
  // Initialize Kokkos
  Kokkos::initialize();

  MeshDatabase mesh(comm,
                    num_global_elements_i,
                    num_global_elements_j,
                    num_global_elements_k,
                    num_procs_i,
                    num_procs_j,
                    num_procs_k);

  const auto sb = mesh.getStructuredBlock();
  const auto part = mesh.getStructuredBlockPart();

  const auto num_owned_elements = mesh.getNumOwnedElements();
  const auto num_remote_elements = mesh.getNumRemoteElements();
  const auto num_owned_and_remote_elements = mesh.getNumElements();

#if PRINT_VERBOSE
  mesh.print(std::cout);
#endif
  
  {
    RCP<TimeMonitor> timerGlobal = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("X) Global")));
    
    // Build Tpetra Maps
    // -----------------
    
    const global_ordinal_type TpetraComputeGlobalNumElements 
      = Teuchos::OrdinalTraits<global_ordinal_type>::invalid();  
    
    // - all internal views are allocated on device; mirror as mesh database is constructed on host  
    const auto mesh_gids_host = mesh.getElementGlobalIDs();
    const auto mesh_gids = Kokkos::create_mirror_view(typename exec_space::memory_space(), mesh.getElementGlobalIDs());
    Kokkos::deep_copy(mesh_gids, mesh_gids_host);

    // for convenience, separate the access to owned and remote gids
    const auto owned_gids  = Kokkos::subview(mesh_gids, local_ordinal_range_type(                 0, num_owned_elements));
    const auto remote_gids = Kokkos::subview(mesh_gids, local_ordinal_range_type(num_owned_elements, num_owned_and_remote_elements));
    
    RCP<const map_type> row_map = rcp(new map_type(TpetraComputeGlobalNumElements, owned_gids, 0, comm));
    RCP<const map_type> col_map = rcp(new map_type(TpetraComputeGlobalNumElements, mesh_gids, 0, comm));
    
#if PRINT_VERBOSE
    row_map->describe(*out);
    col_map->describe(*out);
#endif
    
    // Graph Construction
    // ------------------
    // local graph is constructed on device space
    typedef tpetra_crs_graph_type::local_graph_type local_graph_type;
    typedef local_graph_type::row_map_type::non_const_type rowptr_view_type;
    typedef typename local_graph_type::entries_type colidx_view_type;

    rowptr_view_type rowptr;
    colidx_view_type colidx;
    {
      RCP<TimeMonitor> timerLocalGraphConstruction 
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("0) LocalGraphConstruction")));
      
      rowptr = rowptr_view_type("rowptr", num_owned_elements + 1);
      
      local_ordinal_range_type owned_range_i, owned_range_j, owned_range_k;
      local_ordinal_range_type remote_range_i, remote_range_j, remote_range_k;

      part.getOwnedRange(owned_range_i, owned_range_j, owned_range_k);
      part.getRemoteRange(sb, remote_range_i, remote_range_j, remote_range_k);
      
      // count # of nonzeros per row
      Kokkos::parallel_scan
        (num_owned_elements+1,
         KOKKOS_LAMBDA(const LO &local_idx, 
                       typename rowptr_view_type::non_const_value_type &update, 
                       const bool &final) {
          LO cnt = 0;
          if (local_idx < num_owned_elements) {
            LO i, j, k;
            const GO global_idx = mesh_gids(local_idx);
            sb.idx_to_ijk(global_idx, i, j, k);
            
            cnt = 1; // self
            
            cnt += ((i-1) >= remote_range_i.first );
            cnt += ((i+1) <  remote_range_i.second);
            
            cnt += ((j-1) >= remote_range_j.first );
            cnt += ((j+1) <  remote_range_j.second);
            
            cnt += ((k-1) >= remote_range_k.first );
            cnt += ((k+1) <  remote_range_k.second);
          } 

          rowptr(local_idx) = cnt;           
          if (final)
            rowptr(local_idx) = update;          
          update += cnt;
        });
      
      // the last entry of rowptr is the total number of nonzeros in the local graph
      // mirror to host to use the information in constructing colidx
      auto nnz = Kokkos::subview(rowptr, num_owned_elements);
      const auto nnz_host = Kokkos::create_mirror_view(nnz);
      Kokkos::deep_copy(nnz_host, nnz);
      
      // allocate colidx 
      colidx = colidx_view_type("colidx", nnz_host());
      
      // fill      
      Kokkos::parallel_for
        (num_owned_elements,
         KOKKOS_LAMBDA(const LO &local_idx) {
          LO i, j, k;
          const GO global_idx = mesh_gids(local_idx);
          sb.idx_to_ijk(global_idx, i, j, k);
          
          const LO lbeg = rowptr(local_idx); 
          LO lcnt = lbeg;
          
          // self
          colidx(lcnt++) = local_idx;

          // owned and remote gids are separately sorted

          // sides on i
          { 
            const auto i_minus_one = i-1;
            if (i_minus_one >= owned_range_i.first) {
              colidx(lcnt++) = *lower_bound(&owned_gids(0), &owned_gids(num_owned_elements-1),
                                            sb.ijk_to_idx(i_minus_one,j,k));
            } else if (i_minus_one >= remote_range_i.first) {
              colidx(lcnt++) = *lower_bound(&remote_gids(0), &remote_gids(num_remote_elements-1),
                                            sb.ijk_to_idx(i_minus_one,j,k)) + num_owned_elements;
            }
            const auto i_plus_one = i+1;
            if (i_plus_one < owned_range_i.second) {
              colidx(lcnt++) = *lower_bound(&owned_gids(0), &owned_gids(num_owned_elements-1),
                                            sb.ijk_to_idx(i_plus_one,j,k));
            } else if (i_plus_one < remote_range_i.second) {
              colidx(lcnt++) = *lower_bound(&remote_gids(0), &remote_gids(num_remote_elements-1),
                                            sb.ijk_to_idx(i_plus_one,j,k)) + num_owned_elements;
            }
          }

          // sides on j
          { 
            const auto j_minus_one = j-1;
            if (j_minus_one >= owned_range_j.first) {
              colidx(lcnt++) = *lower_bound(&owned_gids(0), &owned_gids(num_owned_elements-1),
                                            sb.ijk_to_idx(i,j_minus_one,k));
            } else if (j_minus_one >= remote_range_j.first) {
              colidx(lcnt++) = *lower_bound(&remote_gids(0), &remote_gids(num_remote_elements-1),
                                            sb.ijk_to_idx(i,j_minus_one,k)) + num_owned_elements;
            }
            const auto j_plus_one = j+1;
            if (j_plus_one < owned_range_j.second) {
              colidx(lcnt++) = *lower_bound(&owned_gids(0), &owned_gids(num_owned_elements-1),
                                            sb.ijk_to_idx(i,j_plus_one,k));
            } else if (j_plus_one < remote_range_j.second) {
              colidx(lcnt++) = *lower_bound(&remote_gids(0), &remote_gids(num_remote_elements-1),
                                            sb.ijk_to_idx(i,j_plus_one,k)) + num_owned_elements;
            }
          }

          // sides on k
          { 
            const auto k_minus_one = k-1;
            if (k_minus_one >= owned_range_k.first) {
              colidx(lcnt++) = *lower_bound(&owned_gids(0), &owned_gids(num_owned_elements-1),
                                            sb.ijk_to_idx(i,j,k_minus_one));
            } else if (k_minus_one >= remote_range_k.first) {
              colidx(lcnt++) = *lower_bound(&remote_gids(0), &remote_gids(num_remote_elements-1),
                                            sb.ijk_to_idx(i,j,k_minus_one)) + num_owned_elements;
            }
            const auto k_plus_one = k+1;
            if (k_plus_one < owned_range_k.second) {
              colidx(lcnt++) = *lower_bound(&owned_gids(0), &owned_gids(num_owned_elements-1),
                                            sb.ijk_to_idx(i,j,k_plus_one));
            } else if (k_plus_one < remote_range_k.second) {
              colidx(lcnt++) = *lower_bound(&remote_gids(0), &remote_gids(num_remote_elements-1),
                                            sb.ijk_to_idx(i,j,k_plus_one)) + num_owned_elements;
            }
          }

          // sort 
          heap_sort(&colidx(lbeg), lcnt-lbeg);
        });
      
    } // end local graph timer
    
    // Call fillComplete on the crs_graph to finalize it
    RCP<tpetra_crs_graph_type> crs_graph;
    { 
      RCP<TimeMonitor> timerGlobalGraphConstruction
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("1) GlobalGraphConstruction")));
      crs_graph = rcp(new tpetra_crs_graph_type(row_map, col_map, local_graph_type(colidx, rowptr),
                                                Teuchos::null));
    } // end global graph timer
    
#if PRINT_VERBOSE
    crs_graph->describe(*out, Teuchos::VERB_EXTREME);
#endif
    
    // Create BlockCrsMatrix
    RCP<tpetra_blockcrs_type> A = Teuchos::rcp(new tpetra_blockcrs_type(*crs_graph, blocksize));
    {
      RCP<TimeMonitor> timerLocalBlockCrsFill
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("2) LocalBlockCrsFill")));

      // Tpetra BlockCrsMatrix only has high level access functions
      // To fill this on device, we need an access to the meta data of blocks
      const auto rowptr_host = Kokkos::create_mirror_view(rowptr);      
      const auto colidx_host = Kokkos::create_mirror_view(colidx);
      
      Kokkos::deep_copy(rowptr_host, rowptr);
      Kokkos::deep_copy(colidx_host, colidx);

      typedef tpetra_blockcrs_type::little_block_type block_type;
      Kokkos::View<block_type*,exec_space> blocks("blocks", rowptr_host(num_owned_elements));

      const auto blocks_host = Kokkos::create_mirror_view(blocks);
      Kokkos::parallel_for
        (Kokkos::RangePolicy<host_space>(0,num_owned_elements), KOKKOS_LAMBDA(const LO row) {
          for (LO loc=rowptr_host(row);loc<rowptr_host(row+1);++loc) 
            blocks_host(loc) = A->getLocalBlock(row, colidx(loc));
        });    
      Kokkos::deep_copy(blocks, blocks_host);
      
      Kokkos::parallel_for
        (num_owned_elements, KOKKOS_LAMBDA(const LO row) {
          for (LO loc=rowptr(row);loc<rowptr(row+1);++loc) {
            const GO gid_row = mesh_gids_host(row);
            const GO gid_col = mesh_gids_host(colidx(loc));
            
            LO i0, j0, k0, i1, j1, k1;
            sb.idx_to_ijk(gid_row, i0, j0, k0);
            sb.idx_to_ijk(gid_col, i1, j1, k1);
            
            const LO diff_i = i0 - i1;
            const LO diff_j = j0 - j1;
            const LO diff_k = j0 - k1;
            
            auto block = blocks(loc);
            for (LO l0=0;l0<blocksize;++l0) 
              for (LO l1=0;l1<blocksize;++l1) 
                block(l0, l1) = get_block_crs_entry<value_type>(i0, j0, k0,
                                                                diff_i, diff_j, diff_k,
                                                                l0, l1);
          }
        });
    }
    {
      RCP<TimeMonitor> timerBlockCrsFillComplete 
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("3) BlockCrsMatrix FillComplete - currently do nothing")));
      // this function does not exsit 
      //A->fillComplete();
    }
#if PRINT_VERBOSE
    A->describe(*out, Teuchos::VERB_EXTREME);
#endif

    // Create MultiVector
    RCP<tpetra_multivector_type> X = Teuchos::rcp(new tpetra_multivector_type(A->getDomainMap(), nrhs));
    {
      RCP<TimeMonitor> timerMultiVectorFill
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("4) MultiVectorFill")));
      
      auto value = X->getLocalView<typename exec_space::memory_space>();
      auto map = X->getMap()->getLocalMap();
      Kokkos::parallel_for
        (value.extent(0), KOKKOS_LAMBDA(const LO i) {
          const GO gid = map.getGlobalElement(i);
          for (LO j=0,jend=value.extent(1);j<jend;++j) 
            value(i, j) = get_multi_vector_entry<value_type>(gid, j);
        });
    }
    
#if PRINT_VERBOSE
    X->describe(*out, Teuchos::VERB_EXTREME);
#endif

    RCP<tpetra_multivector_type> B = Teuchos::rcp(new tpetra_multivector_type(A->getRangeMap(),  nrhs));

    // matrix vector multiplication
    {
      for (LO iter=0;iter<repeat;++iter) {
        RCP<TimeMonitor> timerBlockCrsApply 
          = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("5) BlockCrs Apply")));        
        A->apply(*X, *B);
      }
    }
      
    // direct conversion: block crs -> point crs 
    {
      RCP<TimeMonitor> timerConvertBlockCrsToPointCrs 
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("6) Conversion from BlockCrs to PointCrs")));
      
    }
    // perform on point crs matrix
    {
      RCP<TimeMonitor> timerPointCrsApply 
        = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("7) PointCrs Apply")));
    }

    // Save crs_matrix as a MatrixMarket file.
    {
      // no function to export block crs 
      // RCP<TimeMonitor> timerMatrixMarket 
      //   = rcp(new TimeMonitor(*TimeMonitor::getNewTimer("Export MatrixMarket ")));
      // std::ofstream ofs("BlockCrsTestMatrix.out", std::ofstream::out);
      // Tpetra::MatrixMarket::Writer<tpetra_blockcrs_type>::writeSparse(ofs, A);
      // ofs.close();
    }      

  } // end global timer

  // Print out timing results.
  TimeMonitor::report(comm.ptr(), std::cout, "");

  // Finalize Kokkos
  Kokkos::finalize();
  
  // This tells the Trilinos test framework that the test passed.
  if(0 == mpiSession.getRank())
    {
      std::cout << "End Result: TEST PASSED" << std::endl;
    }
  
  return EXIT_SUCCESS;
}  // END main()
