INCLUDE(TPLDeclareLibraries)

TPL_DECLARE_LIBRARIES( MUMPS
  REQUIRED_HEADERS dmumps_c.h
  REQUIRED_LIBS_NAMES dmumps pord
  )
