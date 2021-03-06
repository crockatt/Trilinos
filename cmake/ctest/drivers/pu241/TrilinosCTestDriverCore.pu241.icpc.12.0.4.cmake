  
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/../../TrilinosCTestDriverCore.cmake")
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/casl-exclude-trilinos-packages.cmake")

#
# Platform/compiler specific options for godel using gcc
#

MACRO(TRILINOS_SYSTEM_SPECIFIC_CTEST_DRIVER)

  # Base of Trilinos/cmake/ctest then BUILD_DIR_NAME
  SET( CTEST_DASHBOARD_ROOT "${TRILINOS_CMAKE_DIR}/../../${BUILD_DIR_NAME}" )

  SET( CTEST_NOTES_FILES "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}" )
  
  SET( CTEST_BUILD_FLAGS "-j6 -i" )
  SET( CTEST_PARALLEL_LEVEL 6 )

  #SET( CTEST_MEMORYCHECK_COMMAND /usr/bin/valgrind )
  #SET( CTEST_MEMORYCHECK_COMMAND_OPTIONS )
  
  SET_DEFAULT(COMPILER_VERSION "ICPC-12.0.4")

  SET_DEFAULT(Trilinos_ENABLE_KNOWN_EXTERNAL_REPOS_TYPE Nightly)

  IF (BUILD_TYPE STREQUAL DEBUG)
    SET(BUILD_TYPE_OPTIONS_FILE intel-12.0.4-serial-debug-ss-options.cmake)
  ELSEIF (BUILD_TYPE STREQUAL RELEASE)
    SET(BUILD_TYPE_OPTIONS_FILE intel-12.0.4-serial-release-ss-options.cmake)
  ELSE()
    SET(BUILD_TYPE_OPTIONS_FILE)
  ENDIF()
  
  IF (COMM_TYPE STREQUAL MPI)
    SET(TPL_ENABLE_MPI ON)

    MESSAGE(FATAL_ERROR "Error, Intel build does not support MPI yet!")
  
  ELSE()
  
    SET( EXTRA_SYSTEM_CONFIGURE_OPTIONS
      "-DTrilinos_CONFIGURE_OPTIONS_FILE:FILEPATH=${CTEST_SCRIPT_DIRECTORY}/${BUILD_TYPE_OPTIONS_FILE}"
      "-DTPL_ENABLE_MPI:BOOL=OFF"
      ${EXTRA_CONFIGURE_OPTIONS}
      )
  
  ENDIF()

  TRILINOS_CTEST_DRIVER()

ENDMACRO()
