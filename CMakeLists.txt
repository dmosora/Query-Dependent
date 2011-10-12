# Require a specific version of CMake.  I'm using 2.8.5 as I create this file.
# Version 2.8.6 should support Visual Studio IDE creation of Qt classes.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


# =============================================================================
# Establish the project name.  This must be done before other commands.
# CMakeLists in this project can refer to the root source directory as 
# ${TESTCODE_SOURCE_DIR} and to the root binary directory of the project
# as ${TESTCODE_BINARY_DIR}.
# Note: This also names the solution file that is generated for Visual Studio.
PROJECT(VisualizationSystem)


# =============================================================================
# Turn on testing for this project.  Each subproject can now add tests by:
# ADD_TEST(<TestName> ${CMAKE_CURRENT_BINARY_DIR}/SimpleTest Hello)
ENABLE_TESTING()


# =============================================================================
# Setup the version number for the project
SET( TestCode_VERSION_MAJOR 0 )
SET( TestCode_VERSION_MINOR 0 )
SET( TestCode_VERSION_PATCH 0 )
SET( TestCode_VERSION_STRING 
   "${TestCode_VERSION_MAJOR}.${TestCode_VERSION_MINOR}.${TestCode_VERSION_PATCH}"
   )


# =============================================================================
# Setup the base use of Qt in this project.
FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore QtGui QtSql REQUIRED)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
SET( QT_VERSION__VS_ADDIN "" )

# To add support for Qt4 libraries like network or qttest, you need to add 
# both the include files and corresponding libraries. For example, to add 
# support for the network and qttest libraries, you can use: 
#INCLUDE_DIRECTORIES(
#   ${QT_INCLUDE_DIR}
#   ${QT_QTNETWORK_INCLUDE_DIR}
#   ${QT_QTTEST_INCLUDE_DIR}
#)
#
#TARGET_LINK_LIBRARIES(
#   ${QT_LIBRARIES}
#   ${QT_QTNETWORK_LIBRARIES}
#   ${QT_QTTEST_LIBRARIES}
#)


# =============================================================================
# Qt (and perhaps other tools) will generate files outside of the source
# tree.  As a result, the current binary directory is added in order to 
# provide the location of the generated files.
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


# =============================================================================
# Global options
#OPTION( SOME_TEST_OPTION_TO_SEE_WHAT_IT_DOES
#   "This is just a test option.  It doesn't do anything."
#   OFF )


# =============================================================================
# Recurse into the subdirectories representing the various sub-projects.
# This does not actually cause another cmake executable to run. The same 
# process will walk through the project's entire directory structure.
ADD_SUBDIRECTORY (src/Visualization)
ADD_SUBDIRECTORY (src/Tests)

