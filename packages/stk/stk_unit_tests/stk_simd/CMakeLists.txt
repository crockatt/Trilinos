

ASSERT_DEFINED(PACKAGE_SOURCE_DIR CMAKE_CURRENT_SOURCE_DIR)

INCLUDE_DIRECTORIES(${${PARENT_PACKAGE_NAME}_SOURCE_DIR}/stk_simd)
INCLUDE_DIRECTORIES(${${PACKAGE_NAME}_SOURCE_DIR}/stk_simd)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${${PACKAGE_NAME}_BINARY_DIR}/stk_simd)

FILE(GLOB SOURCES *.cpp)

TRIBITS_ADD_EXECUTABLE(
    stk_simd_unit_tests
    SOURCES ${SOURCES}
    COMM serial mpi
    NOEXEPREFIX
    )

TRIBITS_ADD_TEST(
   stk_simd_unit_tests
   NAME STKSimd_Simd
   COMM serial mpi
   PASS_REGULAR_EXPRESSION "PASS"
   FAIL_REGULAR_EXPRESSION "FAIL"
   NUM_MPI_PROCS 1
   NOEXEPREFIX
   )
