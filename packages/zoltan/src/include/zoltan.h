/*****************************************************************************
 * Zoltan Library for Parallel Applications                                  *
 * Copyright (c) 2000,2001,2002, Sandia National Laboratories.               *
 * For more info, see the README file in the top-level Zoltan directory.     *  
 *****************************************************************************/
/*****************************************************************************
 * CVS File Information :
 *    $RCSfile$
 *    $Author$
 *    $Date$
 *    $Revision$
 ****************************************************************************/

#ifndef __ZOLTAN_H
#define __ZOLTAN_H

#include <mpi.h>
#include "zoltan_types.h"
#include "zoltan_align.h"
#include "zoltan_comm.h"
#include "zoltan_mem.h"
#include "zoltan_dd.h"

#ifdef __cplusplus
/* if C++, define the rest of this header file as extern C */
extern "C" {
#endif

#define ZOLTAN_VERSION_NUMBER   1.55
#define BETA_VERSION_2

/*****************************************************************************
 *  Data types and functions describing the interface between the
 *  application and Zoltan.
 */

/*****************************************************************************
 *  Enumerated type used to indicate which function is to be set by
 *  ZOLTAN_Set_Fn.
 */

enum Zoltan_Fn_Type {
  ZOLTAN_NUM_EDGES_FN_TYPE,
  ZOLTAN_NUM_EDGES_MULTI_FN_TYPE,
  ZOLTAN_EDGE_LIST_FN_TYPE,
  ZOLTAN_EDGE_LIST_MULTI_FN_TYPE,
  ZOLTAN_NUM_GEOM_FN_TYPE,
  ZOLTAN_GEOM_MULTI_FN_TYPE,
  ZOLTAN_GEOM_FN_TYPE,
  ZOLTAN_NUM_OBJ_FN_TYPE,
  ZOLTAN_OBJ_LIST_FN_TYPE,
  ZOLTAN_FIRST_OBJ_FN_TYPE,
  ZOLTAN_NEXT_OBJ_FN_TYPE,
  ZOLTAN_NUM_BORDER_OBJ_FN_TYPE,
  ZOLTAN_BORDER_OBJ_LIST_FN_TYPE,
  ZOLTAN_FIRST_BORDER_OBJ_FN_TYPE,
  ZOLTAN_NEXT_BORDER_OBJ_FN_TYPE,
  ZOLTAN_PRE_MIGRATE_PP_FN_TYPE,
  ZOLTAN_MID_MIGRATE_PP_FN_TYPE,
  ZOLTAN_POST_MIGRATE_PP_FN_TYPE,
  ZOLTAN_PRE_MIGRATE_FN_TYPE,
  ZOLTAN_MID_MIGRATE_FN_TYPE,
  ZOLTAN_POST_MIGRATE_FN_TYPE,
  ZOLTAN_OBJ_SIZE_FN_TYPE,
  ZOLTAN_PACK_OBJ_FN_TYPE,
  ZOLTAN_UNPACK_OBJ_FN_TYPE,
  ZOLTAN_NUM_COARSE_OBJ_FN_TYPE,
  ZOLTAN_COARSE_OBJ_LIST_FN_TYPE,
  ZOLTAN_FIRST_COARSE_OBJ_FN_TYPE,
  ZOLTAN_NEXT_COARSE_OBJ_FN_TYPE,
  ZOLTAN_NUM_CHILD_FN_TYPE,
  ZOLTAN_CHILD_LIST_FN_TYPE,
  ZOLTAN_CHILD_WEIGHT_FN_TYPE,
  ZOLTAN_OBJ_SIZE_MULTI_FN_TYPE,
  ZOLTAN_PACK_OBJ_MULTI_FN_TYPE,
  ZOLTAN_UNPACK_OBJ_MULTI_FN_TYPE,
  ZOLTAN_PARTITION_FN_TYPE,
  ZOLTAN_PARTITION_MULTI_FN_TYPE,
  ZOLTAN_GET_PROCESSOR_NAME_FN_TYPE,
  ZOLTAN_NUM_HG_EDGES_FN_TYPE,
  ZOLTAN_NUM_HG_PINS_FN_TYPE,
  ZOLTAN_HG_EDGE_LIST_FN_TYPE,
  ZOLTAN_MAX_FN_TYPES               /*  This entry should always be last. */
};

typedef enum Zoltan_Fn_Type ZOLTAN_FN_TYPE;

/*****************************************************************************
 * Enumerated type used to indicate what type of refinement was used when
 * building a refinement tree.
 */

enum Zoltan_Ref_Type {
  ZOLTAN_OTHER_REF,      /* unspecified type of refinement */
  ZOLTAN_IN_ORDER,       /* user provides the order of the children */
  ZOLTAN_TRI_BISECT,     /* bisection of triangles */
  ZOLTAN_QUAD_QUAD,      /* quadrasection of quadralaterals */
  ZOLTAN_HEX3D_OCT       /* octasection of hexahedra */
};

typedef enum Zoltan_Ref_Type ZOLTAN_REF_TYPE;

/*****************************************************************************
 *  Other common definitions:
 */

struct Zoltan_Struct;

/*****************************************************************************/
/*****************************************************************************/
/**********************  Functions to query application  *********************/
/*****************************************************************************/

/*****************************************************************************/
/*
 *  Function to return, for a list of object IDs, 
 *  the partition numbers the objects are assigned to.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_obj             --  number of objects for which partition values 
 *                            should be returned.
 *    global_id           --  the Global IDs for the objects
 *    local_id            --  the Local IDs for the objects
 *  Output:
 *    parts               --  upon return, an array of partition assignments
 *                            for objects specified in global_id.
 *    *ierr               --  error code
 */

typedef void ZOLTAN_PARTITION_MULTI_FN(
  void *data,              
  int num_gid_entries, 
  int num_lid_entries,
  int num_obj,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *parts,
  int *ierr
);

typedef void ZOLTAN_PARTITION_MULTI_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  int *num_obj,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id, 
  int *parts,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for the object with a given ID,
 *  the partition number the object is assigned to.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  the Global ID for the object
 *    local_id            --  the Local ID for the object
 *  Output:
 *    *ierr               --  error code
 *  Returned value:       --  partition number the object is assigned to.
 */

typedef int ZOLTAN_PARTITION_FN(
  void *data,              
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *ierr
);

typedef int ZOLTAN_PARTITION_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for the object with a given ID,
 *  the object's number of edges (i.e., the number of objects with which
 *  the given object must communicate).
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  the Global ID for the object
 *    local_id            --  the Local ID for the object
 *  Output:
 *    *ierr               --  error code
 *  Returned value:       --  number of neighbor objects.
 */

typedef int ZOLTAN_NUM_EDGES_FN(
  void *data,              
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *ierr
);

typedef int ZOLTAN_NUM_EDGES_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for a list of object with a given IDs,
 *  each object's number of edges (i.e., the number of objects with which
 *  the given object must communicate).
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_obj             --  the number of objects whose IDs are in global_id
 *                            and local_id.
 *    global_ids          --  the Global IDs for the objects
 *    local_ids           --  the Local IDs for the objects
 *  Output:
 *    num_edges           --  array containing the number of edges for each
 *                            object in global_id
 *    *ierr               --  error code
 */

typedef void ZOLTAN_NUM_EDGES_MULTI_FN(
  void *data,              
  int num_gid_entries, 
  int num_lid_entries,
  int num_obj,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *num_edges,
  int *ierr
);

typedef void ZOLTAN_NUM_EDGES_MULTI_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  int *num_obj,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id, 
  int *num_edges,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for the object with a given ID, 
 *  the object's edge list (i.e., objects with which the given object must
 *  communicate.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  the Global ID for the object
 *    local_id            --  the Local ID for the object
 *    wdim                --  dimension of edge weights, or 0 if
 *                            edge weights are not sought.
 *  Output:
 *    nbor_global_ids     --  Array of Global IDs of neighboring objects.
 *    nbor_procs          --  Array of neighboring procs.
 *    nbor_ewgts          --  Array of edge weights, where 
 *                            nbor_ewgts[i*wdim:(i+1)*wdim-1]
 *                            corresponds to the weight of edge i
 *    ierr                --  error code
 */

typedef void ZOLTAN_EDGE_LIST_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  ZOLTAN_ID_PTR nbor_global_id, 
  int *nbor_procs,
  int wdim, 
  float *nbor_ewgts, 
  int *ierr
);

typedef void ZOLTAN_EDGE_LIST_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id, 
  ZOLTAN_ID_PTR nbor_global_id,
  int *nbor_procs, 
  int *wdim, 
  float *nbor_ewgts, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for an array of objects,
 *  each object's edge list (i.e., objects with which the given object must
 *  communicate.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_obj             --  the number of objects whose IDs are in global_id
 *                            and local_id.
 *    global_ids          --  the Global IDs for the objects
 *    local_ids           --  the Local IDs for the objects
 *    num_edges           --  the number of edges for each object.
 *    wdim                --  dimension of edge weights, or 0 if
 *                            edge weights are not sought.
 *  Output:
 *    nbor_global_ids     --  Array of Global IDs of neighboring objects.
 *                            Nbors are stored consecutively; 
 *                            nbor_global_ids[sum:sum+num_edges[i]-1],
 *                            sum = sum j from 0 to i-1 of num_edges[j],
 *                            holds nbors for the i-th global_id.
 *    nbor_procs          --  Array of neighboring procs.  Storage is parallel
 *                            to nbor_global_ids.
 *    nbor_ewgts          --  Array of edge weights, where 
 *                            nbor_ewgts[sum*wdim:(num_edges[i]+sum)*wdim-1],
 *                            sum = sum j from 0 to i-1 of num_edges[j],
 *                            corresponds to the weights for edges of the 
 *                            i-th global_id.
 *    ierr                --  error code
 */

typedef void ZOLTAN_EDGE_LIST_MULTI_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  int num_obj,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids,
  int *num_edges,
  ZOLTAN_ID_PTR nbor_global_id, 
  int *nbor_procs,
  int wdim, 
  float *nbor_ewgts, 
  int *ierr
);

typedef void ZOLTAN_EDGE_LIST_MULTI_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  int *num_obj,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids, 
  int *num_edges,
  ZOLTAN_ID_PTR nbor_global_id,
  int *nbor_procs, 
  int *wdim, 
  float *nbor_ewgts, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return
 *  the number of geometry fields per object (e.g., the number of values
 *  used to express the coordinates of the object).
 *  Input:  
 *    data                --  pointer to user defined data structure
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the number of geometry fields.
 */

typedef int ZOLTAN_NUM_GEOM_FN(
  void *data, 
  int *ierr
);

typedef int ZOLTAN_NUM_GEOM_FORT_FN(
  void *data, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return the geometry information (e.g., coordinates) for 
 *  all objects.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_obj             --  number of objects whose coordinates are needed.
 *    global_id           --  array of Global IDs for the objects
 *    local_id            --  array of Local IDs for the objects; 
 *                            NULL if num_lid_entries == 0.
 *    num_dim             --  dimension of coordinates for each object.
 *  Output:
 *    geom_vec            --  the geometry info for the objects; 
 *                            (e.g., coordinates)
 *                            If num_dim == n, geom_vec[i*n+j] is the 
 *                            jth coordinate for object i.
 *    ierr                --  error code
 */

typedef void ZOLTAN_GEOM_MULTI_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  int num_obj,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int num_dim,
  double *geom_vec, 
  int *ierr
);

typedef void ZOLTAN_GEOM_MULTI_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  int *num_obj,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *num_dim,
  double *geom_vec, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for the object with a given ID,
 *  the geometry information for the object (e.g., coordinates).
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  the Global ID for the object
 *    local_id            --  the Local ID for the object
 *  Output:
 *    geom_vec            --  the geometry info for the object
 *                            (e.g., coordinates)
 *    ierr                --  error code
 */

typedef void ZOLTAN_GEOM_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  double *geom_vec, 
  int *ierr
);

typedef void ZOLTAN_GEOM_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  double *geom_vec, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return, for the calling processor, the number of objects 
 *  located in that processor's memory.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the number of local objects.
 */

typedef int ZOLTAN_NUM_OBJ_FN(
  void *data, 
  int *ierr
);

typedef int ZOLTAN_NUM_OBJ_FORT_FN(
  void *data, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return a list of all local objects on a processor.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    wdim                --  dimension of object weights, or 0 if
 *                            object weights are not sought. 
 *  Output:
 *    global_ids          --  array of Global IDs of all objects on the
 *                            processor.
 *    local_ids           --  array of Local IDs of all objects on the
 *                            processor.
 *    objwgts             --  objwgts[i*wdim:(i+1)*wdim-1] correponds
 *                            to the weight of object i 
 *    ierr                --  error code
 */

typedef void ZOLTAN_OBJ_LIST_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids,
  int wdim, 
  float *objwgts, 
  int *ierr
);

typedef void ZOLTAN_OBJ_LIST_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids,
  int *wdim, 
  float *objwgts,
  int *ierr
);

/*****************************************************************************/
/*
 *  Iterator function for local objects; return the first local object on
 *  the processor.  This function should be used with ZOLTAN_NEXT_OBJ_FN.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    wdim                --  dimension of object weight, or 0 if
 *                            the weight is not sought.
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *  Output:
 *    first_global_id     --  Global ID of the first object; NULL if no
 *                            objects.
 *    first_local_id      --  Local ID of the first object; NULL if no
 *                            objects.
 *    first_obj_wgt       --  weight vector for first object
 *                            (undefined if wdim=0)
 *    ierr                --  error code
 *  Returned value:       --  1 if a valid object is returned; 
 *                            0 if no more objects exist on the processor.
 */

typedef int ZOLTAN_FIRST_OBJ_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR first_global_id,
  ZOLTAN_ID_PTR first_local_id, 
  int wdim, 
  float *first_obj_wgt, 
  int *ierr
);

typedef int ZOLTAN_FIRST_OBJ_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR first_global_id,
  ZOLTAN_ID_PTR first_local_id, 
  int *wdim,
  float *first_obj_wgt, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Iterator function for local objects; return the next local object.
 *  This function should be used with ZOLTAN_FIRST_OBJ_FN.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  Global ID of the previous object.
 *    local_id            --  Local ID of the previous object.
 *    wdim                --  dimension of object weight, or 0 if
 *                             the weight is not sought.
 *  Output:
 *    next_global_id      --  Global ID of the next object; NULL if no
 *                            more objects.
 *    next_local_id       --  Local ID of the next object; NULL if no
 *                            more objects.
 *    next_obj_wgt        --  weight vector for the next object
 *                            (undefined if wdim=0)
 *    ierr                --  error code
 *  Returned value:       --  1 if a valid object is returned; 0 if
 *                            no more objects exist (i.e., global_id is
 *                            the last object).
 */

typedef int ZOLTAN_NEXT_OBJ_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  ZOLTAN_ID_PTR next_global_id, 
  ZOLTAN_ID_PTR next_local_id,
  int wdim, 
  float *next_obj_wgt, 
  int *ierr
);

typedef int ZOLTAN_NEXT_OBJ_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  ZOLTAN_ID_PTR next_global_id,
  ZOLTAN_ID_PTR next_local_id,
  int *wdim, 
  float *next_obj_wgt, 
  int *ierr
);


/*****************************************************************************/
/*
 *  Function to return the size (in bytes) of data associated with an object.
 *  
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  the Global ID for the object
 *    local_id            --  the Local ID for the object
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the size of data of the object
 *                            corresponding to global_id
 */

typedef int ZOLTAN_OBJ_SIZE_FN(
  void *data,
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *ierr
);

typedef int ZOLTAN_OBJ_SIZE_FORT_FN(
  void *data,
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id, 
  ZOLTAN_ID_PTR local_id,
  int *ierr
);

/*****************************************************************************/
/* 
 *  MULTI-ID version of ZOLTAN_OBJ_SIZE_FN 
 *  Function to return the size (in bytes) of data associated with each of
 *  multiple objects.
 * 
 *  Input:
 *    data                --  pointer to user-defined data structure.
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_ids             --  number of objects whose size is sought
 *    global_ids          --  the Global IDs for the objects 
 *    local_ids           --  the Local IDs for the objects
 *  Output:
 *    num_bytes           --  array of sizes (in bytes) for the given IDs 
 *    ierr                --  Zoltan error code
 */

typedef void ZOLTAN_OBJ_SIZE_MULTI_FN(
  void *data,
  int num_gid_entries,
  int num_lid_entries,
  int num_ids,
  ZOLTAN_ID_PTR global_ids,
  ZOLTAN_ID_PTR local_ids,
  int *num_bytes,
  int *ierr);

typedef void ZOLTAN_OBJ_SIZE_MULTI_FORT_FN(
  void *data,
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_ids,
  ZOLTAN_ID_PTR global_ids,
  ZOLTAN_ID_PTR local_ids,
  int *num_bytes,
  int *ierr);


/*****************************************************************************/
/*
 *  Function to pack data to be migrated for the given object.
 *  This function is needed only when the application wants the load-balancer 
 *  to help migrate the data.  It packs all data related to the given object
 *  into a communication buffer, the starting address of which is provided
 *  by the load-balancer.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  Global ID of the object to be packed.
 *    local_id            --  Local ID of the object to be packed.
 *    dest_proc           --  Processor ID of the destination processor.
 *    size                --  number of bytes allowed for the object to
 *                            be packed.
 *    buf                 --  starting address of buffer into which to
 *                            pack the object.
 *  Output:
 *    buf                 --  the buffer is rewritten with the packed
 *                            data.
 *    ierr                --  error code
 */

typedef void ZOLTAN_PACK_OBJ_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int dest_proc,
  int size,
  char *buf,
  int *ierr
);

typedef void ZOLTAN_PACK_OBJ_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int *dest_proc,
  int *size,
  char *buf,
  int *ierr
);

/*****************************************************************************/
/*
 *  MULTI-ID version of ZOLTAN_PACK_OBJ_FN 
 *  Function to pack data for multiple given objects.
 *
 *  Input:
 *    data                --  pointer to user-defined data structure.
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_ids             --  number of objects whose data is to be packed
 *    global_ids          --  the Global IDs for the objects 
 *    local_ids           --  the Local IDs for the objects
 *    dest_proc           --  Processor IDs of the destination processor for the
 *                            objects.
 *    size                --  number of bytes allowed for each object to
 *                            be packed.
 *                            size[i] = # of bytes to store the i-th object's 
 *                            data.  Each size includes padding for alignment.
 *    index               --  Indices into buf giving the starting location 
 *                            of each object's data;
 *                            data for the i-th object are stored in
 *                              buf[index[i]],
 *                              buf[index[i]+1], ...,
 *                              buf[index[i]+size[i]-1].
 *                            Since Zoltan adds some tag information
 *                            to packed data, index[i] != sum[j=0,i-1](size[j]) 
 *    buf                 --  starting address of buffer into which to
 *                            pack the object.
 *  Output:
 *    buf                 --  the buffer is rewritten with the packed
 *                            data.
 *    ierr                --  error code
 */

typedef void ZOLTAN_PACK_OBJ_MULTI_FN(
  void *data,
  int num_gid_entries,
  int num_lid_entries,
  int num_ids,
  ZOLTAN_ID_PTR global_ids,
  ZOLTAN_ID_PTR local_ids,
  int *dest_proc,
  int *size,
  int *index,
  char *buffer,
  int *ierr
);

typedef void ZOLTAN_PACK_OBJ_MULTI_FORT_FN(
  void *data,
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_ids,
  ZOLTAN_ID_PTR global_ids,
  ZOLTAN_ID_PTR local_ids,
  int *dest_proc,
  int *size,
  int *index,
  char *buffer,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to unpack data for an object migrated to a new processor.
 *  This function is needed only when the application wants the load-balancer 
 *  to help migrate the data.  The data is stored in a buffer (char *); the
 *  size of the data for the object is included.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    global_id           --  Global ID of the object to be unpacked.
 *    size                --  number of bytes in the buffer for the
 *                            object.
 *    buf                 --  starting address of buffer into which to
 *                            pack the object.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_UNPACK_OBJ_FN(
  void *data, 
  int num_gid_entries,
  ZOLTAN_ID_PTR global_id, 
  int size,
  char *buf,
  int *ierr
);

typedef void ZOLTAN_UNPACK_OBJ_FORT_FN(
  void *data,
  int *num_gid_entries, 
  ZOLTAN_ID_PTR global_id,
  int *size,
  char *buf,
  int *ierr
);

/*****************************************************************************/

/*  
 * MULTI-ID version of ZOLTAN_UNPACK_OBJ_FN 
 *  Function to unpack data for an object migrated to a new processor.
 *
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_ids             --  number of objects whose data is to be unpacked
 *    global_id           --  Global ID of the objects
 *    size                --  number of bytes in the buffer for the
 *                            objects.
 *                            size[i] = # of bytes to store the i-th ID's data.
 *                            Each size includes padding for alignment.
 *    index               --  Pointers into buf giving the starting location of
 *                            each object's data;
 *                            data for the i-th ID are stored in
 *                              buf[index[i]],
 *                              buf[index[i]+1], ...,
 *                              buf[index[i]+size[i]-1].
 *                            Since Zoltan adds some tag information
 *                            to packed data,
 *                              index[i] != sum[j=0,i-1](size[j]) 
 *    buf                 --  starting address of buffer from which to
 *                            unpack the objects.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_UNPACK_OBJ_MULTI_FN(
  void *data,
  int num_gid_entries,
  int num_ids,
  ZOLTAN_ID_PTR global_ids,
  int *size,
  int *index,
  char *buffer,
  int *ierr
);

typedef void ZOLTAN_UNPACK_OBJ_MULTI_FORT_FN(
  void *data,
  int *num_gid_entries,
  int *num_ids,
  ZOLTAN_ID_PTR global_ids,
  int *size,
  int *index,
  char *buffer,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function called as a pre-processor to migration; it includes partition
 *  as well as processor information.  This function is 
 *  optional, and is used only when the application wants Zoltan
 *  to help migrate the data.  The application can perform any type of 
 *  pre-processing in this function.
 *
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  Number of objects to be imported.
 *    import_global_ids   --  Global IDs of objects to be imported.
 *    import_local_ids    --  Local IDs of objects to be imported.
 *    import_procs        --  Processor IDs of importing processors.
 *    import_to_part      --  Partition numbers to which objs are imported.
 *    num_export          --  Number of objects to be exported.
 *    export_global_ids   --  Global IDs of objects to be exported.
 *    export_local_ids    --  Local IDs of objects to be exported.
 *    export_procs        --  Processor IDs of processors to receive
 *                            the objects.
 *    export_to_part      --  Partition numbers to which objs are exported.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_PRE_MIGRATE_PP_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int *import_to_part,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs,
  int *export_to_part,
  int *ierr
);

typedef void ZOLTAN_PRE_MIGRATE_PP_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids, 
  int *import_procs,
  int *import_to_part,
  int *num_export, 
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids, 
  int *export_procs,
  int *export_to_part,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function called between the packing and unpacking phases of data migration.
 *  It includes partition as well as processor information.
 *  Within Zoltan_Migrate, the data to be migrated is packed and 
 *  communicated; then this function is called (if specified). This function is 
 *  optional, and is used only when the application wants Zoltan
 *  to help migrate the data.  The application can perform any type of 
 *  processing in this function.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  Number of objects to be imported.
 *    import_global_ids   --  Global IDs of objects to be imported.
 *    import_local_ids    --  Local IDs of objects to be imported.
 *    import_procs        --  Processor IDs of importing processors.
 *    import_to_part      --  Partition numbers to which objs are imported.
 *    num_export          --  Number of objects to be exported.
 *    export_global_ids   --  Global IDs of objects to be exported.
 *    export_local_ids    --  Local IDs of objects to be exported.
 *    export_procs        --  Processor IDs of processors to receive
 *                            the objects.
 *    export_to_part      --  Partition numbers to which objs are exported.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_MID_MIGRATE_PP_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int *import_to_part,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs,
  int *export_to_part,
  int *ierr
);

typedef void ZOLTAN_MID_MIGRATE_PP_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids, 
  int *import_procs,
  int *import_to_part,
  int *num_export, 
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids, 
  int *export_procs,
  int *export_to_part,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function called as a post-processor to the migration.  It includes 
 *  partition as well as processor information.  This function is 
 *  optional, and is used only when the application wants Zoltan
 *  to help migrate the data.  The application can perform any type of 
 *  post-processing in this function.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  Number of objects to be imported.
 *    import_global_ids   --  Global IDs of objects to be imported.
 *    import_local_ids    --  Local IDs of objects to be imported.
 *    import_procs        --  Processor IDs of importing processors.
 *    import_to_part      --  Partition numbers to which objs are imported.
 *    num_export          --  Number of objects to be exported.
 *    export_global_ids   --  Global IDs of objects to be exported.
 *    export_local_ids    --  Local IDs of objects to be exported.
 *    export_procs        --  Processor IDs of processors to receive
 *                            the objects.
 *    export_to_part      --  Partition numbers to which objs are exported.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_POST_MIGRATE_PP_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int *import_to_part,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs,
  int *export_to_part,
  int *ierr
);

typedef void ZOLTAN_POST_MIGRATE_PP_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids, 
  int *import_procs,
  int *import_to_part,
  int *num_export, 
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids, 
  int *export_procs,
  int *export_to_part,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function called as a pre-processor to migration; it includes only
 *  processor information.  This function is 
 *  optional, and is used only when the application wants Zoltan
 *  to help migrate the data.  The application can perform any type of 
 *  pre-processing in this function.
 *
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  Number of objects to be imported.
 *    import_global_ids   --  Global IDs of objects to be imported.
 *    import_local_ids    --  Local IDs of objects to be imported.
 *    import_procs        --  Processor IDs of importing processors.
 *    num_export          --  Number of objects to be exported.
 *    export_global_ids   --  Global IDs of objects to be exported.
 *    export_local_ids    --  Local IDs of objects to be exported.
 *    export_procs        --  Processor IDs of processors to receive
 *                            the objects.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_PRE_MIGRATE_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs,
  int *ierr
);

typedef void ZOLTAN_PRE_MIGRATE_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids, 
  int *import_procs,
  int *num_export, 
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids, 
  int *export_procs,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function called between the packing and unpacking phases of data migration.
 *  It includes only processor information.
 *  Within Zoltan_Migrate, the data to be migrated is packed and 
 *  communicated; then this function is called (if specified). This function is 
 *  optional, and is used only when the application wants Zoltan
 *  to help migrate the data.  The application can perform any type of 
 *  processing in this function.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  Number of objects to be imported.
 *    import_global_ids   --  Global IDs of objects to be imported.
 *    import_local_ids    --  Local IDs of objects to be imported.
 *    import_procs        --  Processor IDs of importing processors.
 *    num_export          --  Number of objects to be exported.
 *    export_global_ids   --  Global IDs of objects to be exported.
 *    export_local_ids    --  Local IDs of objects to be exported.
 *    export_procs        --  Processor IDs of processors to receive
 *                            the objects.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_MID_MIGRATE_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs,
  int *ierr
);

typedef void ZOLTAN_MID_MIGRATE_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids, 
  int *import_procs,
  int *num_export, 
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids, 
  int *export_procs,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function called as a post-processor to the migration.  It includes 
 *  only processor information.  This function is 
 *  optional, and is used only when the application wants Zoltan
 *  to help migrate the data.  The application can perform any type of 
 *  post-processing in this function.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  Number of objects to be imported.
 *    import_global_ids   --  Global IDs of objects to be imported.
 *    import_local_ids    --  Local IDs of objects to be imported.
 *    import_procs        --  Processor IDs of importing processors.
 *    num_export          --  Number of objects to be exported.
 *    export_global_ids   --  Global IDs of objects to be exported.
 *    export_local_ids    --  Local IDs of objects to be exported.
 *    export_procs        --  Processor IDs of processors to receive
 *                            the objects.
 *  Output:
 *    ierr                --  error code
 */

typedef void ZOLTAN_POST_MIGRATE_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs,
  int *ierr
);

typedef void ZOLTAN_POST_MIGRATE_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids, 
  int *import_procs,
  int *num_export, 
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids, 
  int *export_procs,
  int *ierr
);
/*****************************************************************************/
/*  Function to get the name of the physical processor on which
 *  the current process is running. 
 *
 *  Input:  
 *    data                -- pointer to user defined data structure
 *
 *  Output:
 *    name                -- name of the processor 
 *    length              -- length of the name
 *    ierr                -- error code
 */

typedef void ZOLTAN_GET_PROCESSOR_NAME_FN(
  void *data,
  char *name,
  int *length, 
  int *ierr
);

/* F90 Interface missing here. */


/*****************************************************************************/
/*
 *  Function to return the number of objects (elements) in the initial coarse
 *  grid; used for initialization of the refinement tree.
 *  Input:
 *    data                --  pointer to user defined data structure
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the number of initial elements.
 */

typedef int ZOLTAN_NUM_COARSE_OBJ_FN(
  void *data, 
  int *ierr
);

typedef int ZOLTAN_NUM_COARSE_OBJ_FORT_FN(
  void *data, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return a list of all objects (elements) in the initial coarse
 *  grid.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *  Output:
 *    global_ids          --  array of Global IDs of all coarse objects
 *    local_ids           --  array of Local IDs of all coarse objects
 *    assigned            --  array indicating processor assignment.
 *                            1 if the object is currently
 *                            assigned to this processor; 0 otherwise.
 *                            For elements that have been refined, it
 *                            is ignored.
 *    num_vert            --  array containing the number of vertices
 *                            for each object
 *    vertices            --  array containing the vertices for each 
 *                            object.  If the sum of the number of
 *                            vertices for objects 0 through i-1 is N,
 *                            then the vertices for object i are in
 *                            vertices[N:N+num_vert[i]]
 *    in_order            --  1 if the user is providing the objects in
 *                              the order in which they should be used
 *                            0 if the order should be determined
 *                              automatically
 *    in_vertex           --  array containing the "in" vertex for each
 *                            object, if the user provides them.  It is
 *                            ignored if in_order==0.  For any with the
 *                            value -1, a vertex will be selected
 *                            automatically
 *    out_vertex          --  array containing the "out" vertex for each
 *                            object; same provisions as in_vertex
 *    ierr                --  error code
 */

typedef void ZOLTAN_COARSE_OBJ_LIST_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_ids,
  ZOLTAN_ID_PTR local_ids,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  int *in_order,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

typedef void ZOLTAN_COARSE_OBJ_LIST_FORT_FN(
  void *data,
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  int *in_order,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

/*****************************************************************************/
/*
 *  Iterator function for coarse objects; return the first coarse object.
 *  This function should be used with ZOLTAN_NEXT_COARSE_OBJ_FN.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *  Output:
 *    global_id           --  Global ID of the first coarse object
 *    local_id            --  Local ID of the first coarse object
 *    assigned            --  indicates processor assignment.
 *                            1 if the object is currently
 *                            assigned to this processor; 0 otherwise.
 *                            For elements that have been refined, it
 *                            is ignored.
 *    num_vert            --  number of vertices in the first object
 *    vertices            --  array containing the vertices of the first
                              coarse object
 *    in_order            --  1 if the user will be providing the elements
 *                              in the order in which they should be used
 *                            0 if the order should be determined
 *                              automatically
 *    in_vertex           --  the "in" vertex of the first coarse object.
 *                            It is ignored if in_order==0.  If the
 *                            value is -1, a vertex will be selected
 *                            automatically
 *    out_vertex          --  array containing the "out" vertex for the
 *                            first object; same provisions as in_vertex
 *    ierr                --  error code
 *  Returned value:       --  1 if a valid object is returned; 0 if
 *                            no more objects exist on the processor.
 */

typedef int ZOLTAN_FIRST_COARSE_OBJ_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  int *in_order,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

typedef int ZOLTAN_FIRST_COARSE_OBJ_FORT_FN(
  void *data,
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  int *in_order,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

/*****************************************************************************/
/*
 *  Iterator function for coarse objects; return the next coarse object.
 *  This function should be used with ZOLTAN_FIRST_COARSE_OBJ_FN.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *  Output:
 *    global_id           --  Global ID of the previous coarse object
 *    local_id            --  Local ID of the previous coarse object
 *    next_global_id      --  Global ID of the next coarse object
 *    next_local_id       --  Local ID of the next coarse object
 *    assigned            --  indicates processor assignment.
 *                            1 if the object is currently
 *                            assigned to this processor; 0 otherwise.
 *                            For elements that have been refined, it
 *                            is ignored.
 *    num_vert            --  number of vertices in the next object
 *    vertices            --  array containing the vertices of the next
 *                            coarse object
 *    in_vertex           --  the "in" vertex of the next coarse object.
 *                            It is ignored if in_order==0 in the call
 *                            to ZOLTAN_FIRST_COARSE_OBJ_FN.  If the
 *                            value is -1, a vertex will be selected
 *                            automatically
 *    out_vertex          --  the "out" vertex for the next object;
 *                            same provisions as in_vertex
 *    ierr                --  error code
 *  Returned value:       --  1 if a valid object is returned; 0 if
 *                            no more objects exist on the processor.
 */

typedef int ZOLTAN_NEXT_COARSE_OBJ_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  ZOLTAN_ID_PTR next_global_id,
  ZOLTAN_ID_PTR next_local_id,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

typedef int ZOLTAN_NEXT_COARSE_OBJ_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  ZOLTAN_ID_PTR next_global_id,
  ZOLTAN_ID_PTR next_local_id,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return the number of children of an element; used for
 *  building a refinement tree.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  Global ID of the object whose number of
 *                            children is requested
 *    local_id            --  Local ID of the object whose number of
 *                            children is requested
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the number of children
 */

typedef int ZOLTAN_NUM_CHILD_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int *ierr
);

typedef int ZOLTAN_NUM_CHILD_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return a list of all children of an object.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    parent_gid          --  Global ID of the object whose children
 *                            are requested
 *    parent_lid          --  Local ID of the object whose children
 *                            are requested
 *  Output:
 *    child_gids          --  array of Global IDs of the children
 *    child_lids          --  array of Local IDs of the children
 *    assigned            --  array indicating processor assignment.
 *                            1 if the child object is currently
 *                            assigned to this processor; 0 otherwise.
 *                            For elements that have been refined, it
 *                            is ignored.
 *    num_vert            --  array containing the number of vertices
 *                            for each child
 *    vertices            --  array containing the vertices for each 
 *                            child.  If the sum of the number of
 *                            vertices for children 0 through i-1 is N,
 *                            then the vertices for child i are in
 *                            vertices[N:N+num_vert[i]]
 *    ref_type            --  indicates what type of refinement was
 *                            used to create the children
 *    in_vertex           --  array containing the "in" vertex for each
 *                            child, if the user provides them.  It is
 *                            ignored if ref_type!=ZOLTAN_IN_ORDER.  For any
 *                            with the value -1, a vertex will be selected
 *                            automatically
 *    out_vertex          --  array containing the "out" vertex for each
 *                            child; same provisions as in_vertex
 *    ierr                --  error code
 */

typedef void ZOLTAN_CHILD_LIST_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR parent_gid,
  ZOLTAN_ID_PTR parent_lid,
  ZOLTAN_ID_PTR child_gids,
  ZOLTAN_ID_PTR child_lids,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  ZOLTAN_REF_TYPE *ref_type,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

typedef void ZOLTAN_CHILD_LIST_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR parent_gid,
  ZOLTAN_ID_PTR parent_lid,
  ZOLTAN_ID_PTR child_gids,
  ZOLTAN_ID_PTR child_lids,
  int *assigned,
  int *num_vert,
  ZOLTAN_ID_PTR vertices,
  ZOLTAN_REF_TYPE *ref_type,
  ZOLTAN_ID_PTR in_vertex,
  ZOLTAN_ID_PTR out_vertex,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return the weight of a child object.
 *  Input:
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  Global ID of the object whose weight
 *                            is requested
 *    local_id            --  Local ID of the object whose weight
 *                            is requested
 *    wdim                --  dimension of object weight, or 0 if
 *                            the weight is not sought.
 *  Output:
 *    obj_wgt             --  weight vector for the object
 *                            (undefined if wdim=0)
 *    ierr                --  error code
 */

typedef void ZOLTAN_CHILD_WEIGHT_FN(
  void *data, 
  int num_gid_entries,
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int wgt_dim,
  float *obj_wgt,
  int *ierr
);

typedef void ZOLTAN_CHILD_WEIGHT_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id,
  int *wgt_dim,
  float *obj_wgt,
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return
 *  the number of hyperedges on the processor.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the number of hyperedges
 */

typedef int ZOLTAN_NUM_HG_EDGES_FN(
  void *data, 
  int *ierr
);

typedef int ZOLTAN_NUM_HG_EDGES_FORT_FN(
  void *data, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return
 *  the number of pins in the local hypergraph.
 *  Number of pins = sum over all local hyperedges of number of vertices 
 *  per hyperedge.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  number of pins in the local hypergraph
 */

typedef int ZOLTAN_NUM_HG_PINS_FN(
  void *data, 
  int *ierr
);

typedef int ZOLTAN_NUM_HG_PINS_FORT_FN(
  void *data, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return a list of hyperedges on the processor.
 *  Each edge is expressed by three fields: a number of vertices in the
 *  edge, a list of vertices in the edge, and the processor assignment of
 *  each vertex in the edge.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    ewgt_dim            --  number of edge weights per edge to be returned.
 *    nedge               --  number of hyperedges to be returned (as
 *                            specified by ZOLTAN_NUM_HG_EDGES_FN.
 *    maxsize             --  maximum total number of vertices that may
 *                            be specified; this field was used by Zoltan
 *                            in allocating memory for the edges.
 *  Output:
 *    edge_sizes          --  Array containing number of vertices per 
 *                            hyperedge; edge_sizes[i] == number of vertices
 *                            in hyperedge i.
 *    edge_verts          --  Array containing the GIDs of vertices in each
 *                            hyperedge.  Hyperedges are concatinated in the
 *                            array; that is, hyperedge 0 is stored in 
 *                            edge_verts[0] to edge_verts[edge_sizes[0]-1],
 *                            hyperedge 1 is stored in edge_verts[edge_sizes[0]]
 *                            to edge_verts[edge_sizes[0]+edge_sizes[1]-1], etc.
 *    edge_procs          --  Array containing the owning processor of each
 *                            vertex in edge_verts.  edge_procs[i] contains the
 *                            processor rank for the processor owning 
 *                            edge_verts[i].
 *    edge_weights        --  Array containing the edge weights for each
 *                            hyperedge.  edge_weights[0] to 
 *                            edge_weights[ewgt_dim-1] contain weights for 
 *                            hyperedge 0; edge_weights[ewgt_dim] to 
 *                            edge_weights[2*ewgt_dim-1] contain weights for
 *                            hyperedge 1; etc.
 *  Returned value:
 *    ierr                --  error code
 */

typedef int ZOLTAN_HG_EDGE_LIST_FN(
  void *data, 
  int num_gid_entries,
  int ewgt_dim,
  int nedge,
  int maxsize,
  int *edge_sizes,
  ZOLTAN_ID_PTR edge_verts,
  int *edge_procs,
  float *edge_weights
);

typedef int ZOLTAN_HG_EDGE_LIST_FORT_FN(
  void *data, 
  int *num_gid_entries,
  int *ewgt_dim,
  int *nedge,
  int *maxsize,
  int *edge_sizes,
  ZOLTAN_ID_PTR edge_verts,
  int *edge_procs,
  float *edge_weights
);

/*****************************************************************************/
/*
 *  Function to return, for the calling processor, the number of objects 
 *  sharing a subdomain border with a given processor.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    nbor_proc           --  processor ID of the neighboring processor.
 *  Output:
 *    ierr                --  error code
 *  Returned value:       --  the number of local objects.
 */

typedef int ZOLTAN_NUM_BORDER_OBJ_FN(
  void *data, 
  int nbor_proc, 
  int *ierr
);

typedef int ZOLTAN_NUM_BORDER_OBJ_FORT_FN(
  void *data, 
  int *nbor_proc, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Function to return a list of all objects sharing a subdomain border 
 *  with a given processor.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    nbor_proc           --  processor ID of the neighboring processor.
 *    wdim                --  dimension of object weight, or 0 if
 *                            the weights are not sought.
 *  Output:
 *    global_ids          --  array of Global IDs of all objects on the
 *                            processor border with the given neighboring
 *                            processor.
 *    local_ids           --  array of Local IDs of all objects on the 
 *                            processor border with the given neighboring 
 *                            processor.
 *    objwgts            --  objwgts[i*wdim:(i+1)*wdim-1] correponds
 *                            to the weight of object i 
 *                            (objwgts is undefined if wdim=0)
 *    ierr               --  error code
 */

typedef void ZOLTAN_BORDER_OBJ_LIST_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  int nbor_proc,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids,
  int wdim, 
  float *objwgts, 
  int *ierr
);

typedef void ZOLTAN_BORDER_OBJ_LIST_FORT_FN(
  void *data,
  int *num_gid_entries, 
  int *num_lid_entries,
  int *nbor_proc,
  ZOLTAN_ID_PTR global_ids, 
  ZOLTAN_ID_PTR local_ids,
  int *wdim, 
  float *objwgts, 
  int *ierr
);

/*****************************************************************************/
/*
 *  Iterator function for border objects; return the first local object 
 *  along the subdomain boundary with a given processor.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    nbor_proc           --  processor ID of the neighboring processor.
 *    wdim                --  dimension of object weight, or 0 if
 *                                  the weight is not sought.
 *  Output:
 *    first_global_id     --  Global ID of the first object; NULL if no
 *                            objects.
 *    first_local_id      --  Local ID of the first object; NULL if no 
 *                            objects.
 *    first_obj_wgt       --  weight vector for the first object
 *                            (undefined if wdim=0)
 *    ierr                --  error code
 *  Returned value:       --  1 if a valid object is returned; 0 if
 *                            no more objects exist (i.e., global_id is
 *                            the last object).
 */

typedef int ZOLTAN_FIRST_BORDER_OBJ_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries, 
  int nbor_proc,
  ZOLTAN_ID_PTR first_global_id,
  ZOLTAN_ID_PTR first_local_id, 
  int wdim, 
  float *first_obj_wgt,
  int *ierr
);

typedef int ZOLTAN_FIRST_BORDER_OBJ_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries, 
  int *nbor_proc,
  ZOLTAN_ID_PTR first_global_id,
  ZOLTAN_ID_PTR first_local_id, 
  int *wdim, 
  float *first_obj_wgt,
  int *ierr
);

/*****************************************************************************/
/*
 *  Iterator function for border objects; return the next local object 
 *  along the subdomain boundary with a given processor.
 *  Input:  
 *    data                --  pointer to user defined data structure
 *    num_gid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of array entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    global_id           --  Global ID of the previous object.
 *    local_id            --  Local ID of the previous object.
 *    nbor_proc           --  processor ID of the neighboring processor.
 *    wdim                --  dimension of object weight, or 0 if
 *                            the weight is not sought.
 *  Output:
 *    next_global_id      --  Global ID of the next object; NULL if no
 *                            more objects.
 *    next_local_id       --  Local ID of the next object; NULL if no 
 *                            more objects.
 *    next_obj_wgt        --  weight vector for the next object
 *                            (undefined if wdim=0)
 *    ierr                --  error code
 *  Returned value:       --  1 if a valid object is returned; 0 if
 *                            no more objects exist (i.e., global_id is
 *                            the last object).
 */

typedef int ZOLTAN_NEXT_BORDER_OBJ_FN(
  void *data, 
  int num_gid_entries, 
  int num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id, 
  int nbor_proc,
  ZOLTAN_ID_PTR next_global_id,
  ZOLTAN_ID_PTR next_local_id, 
  int wdim, 
  float *next_obj_wgt,
  int *ierr
);

typedef int ZOLTAN_NEXT_BORDER_OBJ_FORT_FN(
  void *data, 
  int *num_gid_entries, 
  int *num_lid_entries,
  ZOLTAN_ID_PTR global_id,
  ZOLTAN_ID_PTR local_id, 
  int *nbor_proc,
  ZOLTAN_ID_PTR next_global_id,
  ZOLTAN_ID_PTR next_local_id, 
  int *wdim, 
  float *next_obj_wgt,
  int *ierr
);

/*****************************************************************************/
/*****************************************************************************/
/*******  Functions to set-up Zoltan load-balancing data structure  **********/
/*****************************************************************************/
/*****************************************************************************/

/*
 *  Function to initialize values needed in load balancing tools, and
 *  returns which version of the library this is. If the application
 *  uses MPI, call this function after calling MPI_Init. If the
 *  application does not use MPI, this function calls MPI_Init for
 *  use by Zoltan. This function returns the version of
 *  the Zoltan library.
 *  Input:
 *    argc                --  Argument count from main()
 *    argv                --  Argument list from main()
 *  Output:
 *    ver                 --  Version of Zoltan library
 *  Returned value:       --  Error code
 */

extern int Zoltan_Initialize(
  int argc,
  char **argv,
  float *ver
);

/*****************************************************************************/
/*
 *  Function to create a Zoltan structure.  May want more than one
 *  structure if using different decompositions with different techniques.
 *  This function allocates and initializes the structure.
 *  Input:
 *    communicator        --  MPI Communicator to be used for this
 *                            Zoltan structure.
 *  Returned value:       --  Pointer to a Zoltan structure.
 *                            If there is an error, NULL is returned.
 *                            Any error in this function should be
 *                            considered fatal.
 */

extern struct Zoltan_Struct *Zoltan_Create(
  MPI_Comm communicator
);

/*****************************************************************************/
/*
 *  Function to free the space associated with a Zoltan structure.
 *  The input pointer is set to NULL when the routine returns.
 *  Input/Output:
 *    zz                  --  Pointer to a Zoltan structure.
 */

extern void Zoltan_Destroy(
  struct Zoltan_Struct **zz
);

/*****************************************************************************/
/*
 *  General function to initialize a given Zoltan callback function.
 *  Input:
 *    zz                  --  Pointer to a Zoltan structure.
 *    fn_type             --  Enum type indicating the function to be
 *                            set.
 *    fn_ptr              --  Pointer to the function to be used in the 
 *                            assignment.
 *    data_ptr            --  Pointer to data that Zoltan will
 *                            pass as an argument to fn(). May be NULL.
 *  Output:
 *    zz                  --  Appropriate field set to value in fn_ptr.
 *  Returned value:       --  Error code
 */

extern int Zoltan_Set_Fn(
  struct Zoltan_Struct *zz,
  ZOLTAN_FN_TYPE fn_type,
  void (*fn_ptr)(void),
  void *data_ptr
);

/*
 *  Functions to initialize specific Zoltan callback functions.  One function
 *  exists for each callback function type, as listed in Zoltan_Fn_Type above.
 *  Use of these specific functions enables stricter type checking of the
 *  callback function types.
 *  Input:
 *    zz                  --  Pointer to a Zoltan structure.
 *    fn_ptr              --  Pointer to the function to be used in the 
 *                            assignment, where FN is one of the
 *                            callback function typedef'ed above.
 *    data_ptr            --  Pointer to data that Zoltan will
 *                            pass as an argument to fn(). May be NULL.
 *  Output:
 *    zz                  --  Appropriate field set to value in fn_ptr.
 *  Returned value:       --  Error code
 */

extern int Zoltan_Set_Partition_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_PARTITION_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Partition_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_PARTITION_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Edges_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_EDGES_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Edges_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_EDGES_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Edge_List_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_EDGE_LIST_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Edge_List_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_EDGE_LIST_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Geom_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_GEOM_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Geom_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_GEOM_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Geom_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_GEOM_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Obj_List_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_OBJ_LIST_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_First_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_FIRST_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Next_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NEXT_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Border_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_BORDER_OBJ_FN *fn_ptr,
  void *data_ptr
);

extern int Zoltan_Set_Border_Obj_List_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_BORDER_OBJ_LIST_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_First_Border_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_FIRST_BORDER_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Next_Border_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NEXT_BORDER_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Pre_Migrate_PP_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_PRE_MIGRATE_PP_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Mid_Migrate_PP_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_MID_MIGRATE_PP_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Post_Migrate_PP_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_POST_MIGRATE_PP_FN *fn_ptr, 
  void *data_ptr
);
extern int Zoltan_Set_Pre_Migrate_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_PRE_MIGRATE_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Mid_Migrate_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_MID_MIGRATE_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Post_Migrate_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_POST_MIGRATE_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Obj_Size_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_OBJ_SIZE_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Obj_Size_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_OBJ_SIZE_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Pack_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_PACK_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Pack_Obj_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_PACK_OBJ_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Unpack_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_UNPACK_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Unpack_Obj_Multi_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_UNPACK_OBJ_MULTI_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Coarse_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_COARSE_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Coarse_Obj_List_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_COARSE_OBJ_LIST_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_First_Coarse_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_FIRST_COARSE_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Next_Coarse_Obj_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NEXT_COARSE_OBJ_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_Child_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_CHILD_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Child_List_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_CHILD_LIST_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Child_Weight_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_CHILD_WEIGHT_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_HG_Edges_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_HG_EDGES_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_Num_HG_Pins_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_NUM_HG_PINS_FN *fn_ptr, 
  void *data_ptr
);

extern int Zoltan_Set_HG_Edge_List_Fn(
  struct Zoltan_Struct *zz, 
  ZOLTAN_HG_EDGE_LIST_FN *fn_ptr, 
  void *data_ptr
);

/*****************************************************************************/
/*
 *  Function to change a parameter value within Zoltan.
 *  Default values will be used for all parameters not explicitly altered
 *  by a call to this routine.
 *
 *  Input
 *    zz                  --  The Zoltan structure to which this
 *                            parameter alteration applies.
 *    name                --  The name of the parameter to have its
 *                            value changed.
 *    val                 --  The new value of the parameter.
 *
 *  Returned value:       --  Error code
 */

extern int Zoltan_Set_Param(
  struct Zoltan_Struct *zz, 
  char *name, 
  char *val
);

/*****************************************************************************/
/*
 *  Function to change a vector parameter value within Zoltan.
 *  Default values will be used for all parameters not explicitly altered
 *  by a call to this routine.
 *
 *  Input
 *    zz                  --  The Zoltan structure to which this
 *                            parameter alteration applies.
 *    name                --  The name of the parameter to have its
 *                            value changed.
 *    val                 --  The new value of the parameter.
 *    index               --  The index of the parameter entry
 *                            to be set. By convention, the
 *                            first index is 0 (not 1).
 *
 *  Returned value:       --  Error code
 */

extern int Zoltan_Set_Param_Vec(
  struct Zoltan_Struct *zz, 
  char *name, 
  char *val,
  int index
);

/*****************************************************************************/
/*
 *  Function to invoke the partitioner.
 *
 *  Input:
 *    zz                  --  The Zoltan structure returned by Zoltan_Create.
 *  Output:
 *    changes             --  This value tells whether the new 
 *                            decomposition computed by Zoltan differs 
 *                            from the one given as input to Zoltan.
 *                            It can be either a one or a zero:
 *                            zero - No changes to the decomposition
 *                                   were made by the partitioning
 *                                   algorithm; migration isn't needed.
 *                            one  - A new decomposition is suggested
 *                                   by the partitioner; migration
 *                                   is needed to establish the new
 *                                   decomposition.
 *    num_gid_entries     --  number of entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *    num_import          --  The number of non-local objects in the 
 *                            processor's new decomposition (i.e.,
 *                            number of objects to be imported).
 *    import_global_ids   --  Pointer to array of Global IDs for the
 *                            objects to be imported.
 *    import_local_ids    --  Pointer to array of Local IDs for the 
 *                            objects to be imported (local to the
 *                            exporting processor).
 *    import_procs        --  Pointer to array of Processor IDs for the
 *                            objects to be imported (processor IDs of
 *                            source processor).
 *    import_to_part      --  Pointer to array of partition numbers to 
 *                            which the imported objects should be assigned.
 *    num_export          --  The number of local objects that must be
 *                            exported from the processor to establish
 *                            the new decomposition.
 *    export_global_ids   --  Pointer to array of Global IDs for the
 *                            objects to be exported from the current
 *                            processor.
 *    export_local_ids    --  Pointer to array of Local IDs for the
 *                            objects to be exported (local to the
 *                            current processor).
 *    export_procs        --  Pointer to array of Processor IDs for the
 *                            objects to be exported (processor IDs of
 *                            destination processors).
 *    export_to_part      --  Pointer to array of partition numbers to 
 *                            which the exported objects should be assigned.
 *  Returned value:       --  Error code
 */

extern int Zoltan_LB_Partition(
  struct Zoltan_Struct *zz,
  int *changes,
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR *import_global_ids,
  ZOLTAN_ID_PTR *import_local_ids,
  int **import_procs,
  int **import_to_part,
  int *num_export,
  ZOLTAN_ID_PTR *export_global_ids,
  ZOLTAN_ID_PTR *export_local_ids,
  int **export_procs,
  int **export_to_part 
);
/*****************************************************************************/
/*
 *  Function to invoke the load-balancer.
 *  Appropriate only when the number of requested partitions is equal to the
 *  number of processors.
 *
 *  Input and output:
 *    Arguments are analogous to Zoltan_LB_Partition.  Arrays import_to_part
 *    and export_to_part are not included, as Zoltan_LB_Balance assumes
 *    partitions and processors are equivalent.
 *  Returned value:       --  Error code
 */

extern int Zoltan_LB_Balance(
  struct Zoltan_Struct *zz,
  int *changes,
  int *num_gid_entries,
  int *num_lid_entries,
  int *num_import,
  ZOLTAN_ID_PTR *import_global_ids,
  ZOLTAN_ID_PTR *import_local_ids,
  int **import_procs,
  int *num_export,
  ZOLTAN_ID_PTR *export_global_ids,
  ZOLTAN_ID_PTR *export_local_ids,
  int **export_procs
);

/*****************************************************************************/
/*
 *  Function to return the bounding box of a partition generated by RCB.
 *  Input:
 *    zz                  --  The Zoltan structure returned by Zoltan_Create.
 *    part                --  The partition number whose bounding box is to
 *                            be returned.
 *  Output:
 *    ndim                --  Number of dimensions in the bounding box.
 *    xmin                --  lower x extent of box
 *    ymin                --  lower y extent of box
 *    zmin                --  lower z extent of box
 *    xmax                --  upper x extent of box
 *    ymax                --  upper y extent of box
 *    zmax                --  upper z extent of box 
 *  Returned value:       --  Error code
 */

int Zoltan_RCB_Box(
  struct Zoltan_Struct *zz,
  int     part,
  int    *ndim,
  double *xmin,
  double *ymin,
  double *zmin,
  double *xmax,
  double *ymax,
  double *zmax
);

/*****************************************************************************/
/*
 *  Routine to compute an ordering (permutation) of the objects.
 *
 *  Input:
 *    zz                  --  The Zoltan structure containing 
 *                            info for this load-balancing invocation.
 *  Output:
 *    num_gid_entries     --  number of entries of type ZOLTAN_ID_TYPE
 *                            in a global ID
 *    num_lid_entries     --  number of entries of type ZOLTAN_ID_TYPE
 *                            in a local ID
 *
 *    gids                --  list of global ids in permuted order
 *
 *    lids                --  list of local ids in permuted order
 *
 *    order_info          --  pointer to an "order struct" that contains
 *                            additional information about an ordering
 *
 *  Returned value:       --  Error code
 */

struct Zoltan_Order_Struct; /* Structure defined in order/order_const.h */

extern int Zoltan_Order(
  struct Zoltan_Struct *zz,
  int *num_gid_entries,
  int *num_lid_entries,
  int num_obj,
  ZOLTAN_ID_PTR global_ids,
  ZOLTAN_ID_PTR local_ids,
  int *rank,
  int *iperm,
  struct Zoltan_Order_Struct *order_info  /* Currently not used. */
);

/*****************************************************************************/
/*
 *  Routine to compute the inverse map:  Given, for each processor, a list
 *  of objects to be received by a processor, compute the list of objects
 *  that processor needs to send to other processors to complete a 
 *  remapping.  Conversely, given a list of objects to be sent to other 
 *  processors, compute the list of objects to be received.
 *
 *  Input:
 *    zz                  --  Zoltan structure for current 
 *                            balance.
 *    num_input           --  Number of objects known to be 
 *                            sent/received.
 *    input_global_ids    --  Array of global IDs for known objects.
 *    input_local_ids     --  Array of local IDs for known objects.
 *    input_procs         --  Array of IDs of processors to/from whom the
 *                            known objects will be sent/received.
 *    input_to_part       --  Array of partition numbers to 
 *                            which the known objects should be assigned.
 *  Output:
 *    num_output          --  The number of objects will be received/sent.
 *    output_global_ids   --  Pointer to array of Global IDs for the
 *                            objects to be received/sent.
 *    output_local_ids    --  Pointer to array of Local IDs for the
 *                            objects to be received/sent.
 *    output_procs        --  Pointer to array of Processor IDs 
 *                            from/to which the output_global_ids will be
 *                            received/sent.
 *    output_to_part      --  Pointer to array of partition numbers to 
 *                            which the output_global_ids should be assigned.
 *  Returned value:       --  Error code
 */


extern int Zoltan_Invert_Lists(
  struct Zoltan_Struct *zz,
  int num_input, 
  ZOLTAN_ID_PTR input_global_ids,
  ZOLTAN_ID_PTR input_local_ids, 
  int *input_procs, 
  int *input_to_part, 
  int *num_output, 
  ZOLTAN_ID_PTR *output_global_ids,
  ZOLTAN_ID_PTR *output_local_ids,
  int **output_procs,
  int **output_to_part
);
/*****************************************************************************/
/*
 *  Wrapper around Zoltan_Invert_Lists, appropriate only when 
 *  number of partitions == number of processors (or when partition information
 *  is not desired).
 *
 *  Input and Output:
 *    Arguments are analogous to Zoltan_Invert_Lists.  Arrays import_to_part
 *    and export_to_part are not included, as Zoltan_Compute_Destinations 
 *    assumes partitions and processors are equivalent.
 *  Returned value:       --  Error code
 */

extern int Zoltan_Compute_Destinations(
  struct Zoltan_Struct *zz,
  int num_input, 
  ZOLTAN_ID_PTR input_global_ids,
  ZOLTAN_ID_PTR input_local_ids, 
  int *input_procs, 
  int *num_output, 
  ZOLTAN_ID_PTR *output_global_ids,
  ZOLTAN_ID_PTR *output_local_ids,
  int **output_procs
);

/*****************************************************************************/
/*
 *  Routine to help perform migration.  Zoltan_Migrate performs the following
 *  operations:
 *  - Call migration pre-processing routine (ZOLTAN_PRE_MIGRATE_PP_FN), if 
 *    specified.
 *  - Call a ZOLTAN_OBJ_SIZE_FN to obtain the size of the migrating objects.
 *  - Call the application-specified object packing routine (ZOLTAN_PACK_OBJ_FN)
 *    for each object to be exported.  
 *  - Develop the communication map to move the objects to other processors.  
 *  - Perform the communication according to the map. 
 *  - Call mid-migration processing routine (ZOLTAN_MID_MIGRATE_PP_FN), if 
 *    specified.
 *  - Call the application-specified object unpacking routine 
 *    (ZOLTAN_UNPACK_OBJ_FN) for each object imported. 
 *  - Call post-migration processing routine (ZOLTAN_POST_MIGRATE_PP_FN), if 
 *    specified.
 *
 *  Input:
 *    zz                  --  Zoltan structure for current 
 *                            balance.
 *    num_import          --  Number of non-local objects assigned to the
 *                            processor in the new decomposition.
 *    import_global_ids   --  Array of global IDs for non-local objects
 *                            assigned to this processor in the new
 *                            decomposition.
 *    import_local_ids    --  Array of local IDs for non-local objects
 *                            assigned to the processor in the new
 *                            decomposition.
 *    import_procs        --  Array of processor IDs of processors owning
 *                            the non-local objects that are assigned to
 *                            this processor in the new decomposition.
 *    import_to_part      --  Pointer to array of partition numbers to 
 *                            which the imported objects should be assigned.
 *    num_export          --  The number of local objects that need to be
 *                            exported from the processor to establish
 *                            the new decomposition.
 *    export_global_ids   --  Array of Global IDs for the objects to be
 *                            exported from the current processor.
 *    export_local_ids    --  Array of Local IDs for the objects to be 
 *                            exported (local to the current processor).
 *    export_procs        --  Array of Processor IDs for the objects to
 *                            be exported (processor IDs of destination
 *                            processor).
 *    export_to_part      --  Pointer to array of partition numbers to 
 *                            which the exported objects should be assigned.
 *  Output:
 *    none                --  The objects are migrated to their new
 *                            processor locations.  The input arrays
 *                            are unchanged.
 *  Returned value:       --  Error code
 */

extern int Zoltan_Migrate(
  struct Zoltan_Struct *zz, 
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int *import_to_part,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs, 
  int *export_to_part);

/*****************************************************************************/
/*
 *  Routine to help perform migration.  Can be used instead of Zoltan_Migrate
 *  if the number of partitions is equal to the number of processors.
 *  Calls ZOLTAN_PRE_MIGRATE_FN, ZOLTAN_MID_MIGRATE_FN, and 
 *  ZOLTAN_POST_MIGRATE_FN.
 *
 *  Input and Output:
 *    Arguments are analogous to Zoltan_Migrate.  Arrays import_to_part
 *    and export_to_part are not included, as Zoltan_Help_Migrate 
 *    assumes partitions and processors are equivalent.
 *  Returned value:       --  Error code
 */

extern int Zoltan_Help_Migrate(
  struct Zoltan_Struct *zz, 
  int num_import,
  ZOLTAN_ID_PTR import_global_ids,
  ZOLTAN_ID_PTR import_local_ids,
  int *import_procs,
  int num_export,
  ZOLTAN_ID_PTR export_global_ids,
  ZOLTAN_ID_PTR export_local_ids,
  int *export_procs);

/*****************************************************************************/
/*
 *  Routine to free the data arrays returned by Zoltan_LB_Partition, 
 *  Zoltan_LB_Balance, Zoltan_Invert_Lists, and 
 *  Zoltan_Compute_Destinations.  The arrays
 *  are freed and the pointers are set to NULL.
 *
 *  Input:
 *    global_ids   --  Pointer to array of global IDs 
 *    local_ids    --  Pointer to array of local IDs 
 *    procs        --  Pointer to array of processor IDs 
 *    to_proc      --  Pointer to array of partition assignments
 *  Returned value:       --  Error code
 */
extern int Zoltan_LB_Free_Part(
  ZOLTAN_ID_PTR *global_ids, 
  ZOLTAN_ID_PTR *local_ids,
  int **procs,
  int **to_part
);

/*****************************************************************************/
/*
 *  Routine to free the data arrays returned by Zoltan_Balance.  The arrays
 *  are freed and the pointers are set to NULL.
 *
 *  Input:
 *    import_global_ids   --  Pointer to array of global IDs for 
 *                            imported objects.
 *    import_local_ids    --  Pointer to array of local IDs for 
 *                            imported objects.
 *    import_procs        --  Pointer to array of processor IDs of 
 *                            imported objects.
 *    export_global_ids   --  Pointer to array of global IDs for 
 *                            exported objects.
 *    export_local_ids    --  Pointer to array of local IDs for 
 *                            exported objects.
 *    export_procs        --  Pointer to array of destination processor
 *                            IDs of exported objects.
 *  Returned value:       --  Error code
 */
extern int Zoltan_LB_Free_Data(
  ZOLTAN_ID_PTR *import_global_ids, 
  ZOLTAN_ID_PTR *import_local_ids,
  int **import_procs,
  ZOLTAN_ID_PTR *export_global_ids, 
  ZOLTAN_ID_PTR *export_local_ids,
  int **export_procs
);

/*****************************************************************************/
/* 
 * Routine to determine which processor and partition a new point should be 
 * assigned to.
 * Note that this only works of the current partition was produced via a
 * geometric algorithm - currently RCB, RIB, HSFC.
 * 
 * Input:
 *   zz                   -- pointer to Zoltan structure
 *   coords               -- vector of coordinates of new point
 *
 * Output:
 *   proc                 -- processor that point should be assigned to
 *   part                 -- partition that point should be assigned to
 *
 *  Returned value:       --  Error code
 */

extern int Zoltan_LB_Point_PP_Assign(
  struct Zoltan_Struct *zz,
  double *coords,
  int *proc,
  int *part
);

/*****************************************************************************/
/* 
 * Routine to determine which processor a new point should be assigned to.
 * Can be used instead of Zoltan_LB_Point_PP_Assign when the number of 
 * partitions equals the number of processors.
 * Note that this only works of the current partition was produced via a
 * geometric algorithm - currently RCB, RIB, HSFC.
 * 
 * Input:
 *    Arguments are analogous to Zoltan_LB_Point_PP_Assign.  
 *    Variable part is not included, as Zoltan_LB_Point_Assign
 *    assumes partitions and processors are equivalent.
 *
 * Output:
 *   proc                 -- processor that point should be assigned to
 *
 * Returned value:       --  Error code
 */

extern int Zoltan_LB_Point_Assign(
  struct Zoltan_Struct *zz,
  double *coords,
  int *proc
);

/*****************************************************************************/
/* 
 * Routine to determine which partitions and processors 
 * a bounding box intersects.
 * Note that this only works of the current partition was produced via a
 * geometric algorithm - currently RCB, RIB, HSFC.
 * 
 * Input:
 *   zz                   -- pointer to Zoltan structure
 *   xmin, ymin, zmin     -- lower left corner of bounding box
 *   xmax, ymax, zmax     -- upper right corner of bounding box
 *
 * Output:
 *   procs                -- list of processors that box intersects.  
 *                           Note: application is
 *                               responsible for ensuring sufficient memory.
 *   numprocs             -- number of processors box intersects
 *   parts                -- list of partitions that box intersects.  
 *                           Note: application is
 *                               responsible for ensuring sufficient memory.
 *   numparts             -- number of partitions box intersects (may differ
 *                           from numprocs).
 *
 * Returned value:       --  Error code
 */

extern int Zoltan_LB_Box_PP_Assign(
  struct Zoltan_Struct *zz,
  double xmin,
  double ymin,
  double zmin,
  double xmax,
  double ymax,
  double zmax,
  int *procs,
  int *numprocs,
  int *parts,
  int *numparts
);

/*****************************************************************************/
/* 
 * Routine to determine which processors a bounding box intersects.
 * Note that this only works of the current partition was produced via a
 * geometric algorithm - currently RCB, RIB, HSFC.
 * 
 * Input:
 *   zz                   -- pointer to Zoltan structure
 *   xmin, ymin, zmin     -- lower left corner of bounding box
 *   xmax, ymax, zmax     -- upper right corner of bounding box
 *
 * Output:
 *   procs                -- list of processors that box intersects.  
 *                           Note: application is
 *                               responsible for ensuring sufficient memory.
 *   numprocs             -- number of processors box intersects
 *
 *  Returned value:       --  Error code
 */

extern int Zoltan_LB_Box_Assign(
  struct Zoltan_Struct *zz,
  double xmin,
  double ymin,
  double zmin,
  double xmax,
  double ymax,
  double zmax,
  int *procs,
  int *numprocs
);

/*****************************************************************************/
/* 
 * Routine to compute statistics about the current balance/partitioning.
 *
 * Input:
 *   zz                   -- pointer to Zoltan structure
 *   print_stats          -- if >0, compute and print max and sum of the metrics
 *
 * Output:
 *   nobj                 -- number of objects (for each processor)
 *   obj_wgt              -- obj_wgt[0:vwgt_dim-1] are the object weights 
 *                           (on each processor)
 *   cut_wgt              -- cut size/weight (for each processor)
 *   nboundary            -- number of boundary objects (for each processor)
 *   nadj                 -- the number of adjacent procs (for each processor)
 *
 * Returned value:        -- error code
 */

extern int Zoltan_LB_Eval(
  struct Zoltan_Struct *zz, 
  int print_stats, 
  int *nobj,
  float *obj_wgt,
  int *ncuts,
  float *cut_wgt, 
  int *nboundary,
  int *nadj
);

/*
 *  Function to set the desired partition sizes. 
 *
 *  Input:
 *    zz            --  The Zoltan structure to which this method
 *                      applies.
 *    global_num    --  Global partition numbers? (0 for local numbers)
 *    len           --  Length of arrays wgt_idx, part_idx, part_sizes
 *    part_ids      --  Array of partition ids (local or global)
 *    wgt_idx       --  Array of indices between 0 and Obj_Wgt_Dim-1
 *    part_sizes    --  Array of floats that gives the desired partition 
 *                      size for each weight and each partition, i.e., 
 *                      part_sizes[i] corresponds to wgt_idx[i] and part_id[i]
 *
 *  Output:
 *    Return value  --  Error code.
 */
extern int Zoltan_LB_Set_Part_Sizes(struct Zoltan_Struct *zz, int global_num,
  int len, int *part_ids, int *wgt_idx, float *part_sizes);

/*
 *  Function to generate data files.
 *
 *  Input:
 *    zz            --  The current Zoltan structure 
 *    fname         --  Basename for files to be generated
 *    base_index    --  Start numbering of nodes and edges at 0 or 1?
 *    gen_geom      --  Write geometry file?
 *    gen_graph     --  Write graph file?
 *    gen_hg        --  Write hypergraph file?
 *
 *  Output:
 *    Return value  --  Error code.
 */
extern int Zoltan_Generate_Files(struct Zoltan_Struct *zz, char *fname, int base_index, int gen_geom, int gen_graph, int gen_hg);

#ifdef __cplusplus
} /* closing bracket for extern "C" */
#endif

#endif /* !__ZOLTAN_H */
