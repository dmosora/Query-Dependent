
# =============================================================================
# Define options specific to this sub-project


# =============================================================================
# Qt (and perhaps other tools) will generate files outside of the source
# tree.  As a result, the current binary directory is added in order to 
# provide the location of the generated files.
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


# =============================================================================
# Setup all of the files that make up this project.
SET(TEST_SRC 
   main.cpp
   Test.cpp
   )

# Only add headers that are for Qt.
SET(TEST_HDR 
   Test.h
   ) 

SET(TEST_FRM
   )

SET(TEST_RES
   )


# =============================================================================
# Process the files so that they go through the Qt preprocessing.
QT4_WRAP_CPP     (TEST_SRC_MOC ${TEST_HDR})
QT4_WRAP_UI      (TEST_FRM_MOC ${TEST_FRM})
QT4_ADD_RESOURCES(TEST_RES_MOC ${TEST_RES})


# =============================================================================
# Add executable that is built from the listed source files and the moc 
# generated files.  Adding the FRM_MOC here is what adds the Qt generated
# headers for ui files to the project.
# Note: This also names the project that is generated for Visual Studio.
ADD_EXECUTABLE (Test
    ${TEST_SRC}
    ${TEST_SRC_MOC}
    ${TEST_FRM_MOC}
    ${TEST_RES_MOC}
)

# Qt Test support
INCLUDE_DIRECTORIES( ${QT_QTTEST_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES( Test ${QT_QTTEST_LIBRARIES})

# Include the libraries that need linked.  This must come after the target.
TARGET_LINK_LIBRARIES( Test ${QT_LIBRARIES} )


# =============================================================================
# Setup some project specific settings.

# This makes the project file compatible with the Qt Visual Studio plugin.  Note
# that the VS_GLOBAL isn't expected to be supported until CMake version 2.8.6.
# Once it is supported, then a specific version of Qt is required, or ir can be 
# left blank.  Leaving it blank will result in a message indicating that it can't
# find the right version and then it will resort to the default version of the
# user.
SET_TARGET_PROPERTIES( Test PROPERTIES VS_KEYWORD "Qt4VSv1.0" )
#SET_TARGET_PROPERTIES( Test PROPERTIES 
#    VS_GLOBAL_QtVersion ${QT_VERSION__VS_ADDIN} )


# =============================================================================
# Set this up as a test
ADD_TEST(GenericTest Test)

# =============================================================================
# Some code from the CMake example
#

# Make sure the compiler can find include files from our Hello library.
#include_directories (${HELLO_SOURCE_DIR}/Hello)

# Make sure the linker can find the Hello library once it is built.
#link_directories (${HELLO_BINARY_DIR}/Hello)

# Link the executable to the Hello library.
#target_link_libraries (helloDemo Hello) 
