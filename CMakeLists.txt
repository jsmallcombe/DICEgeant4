#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(DICE)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
# include_directories(${PROJECT_SOURCE_DIR}/include 
#                     ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
# #

add_executable(DICE10 DICE10.cc ${sources} ${headers})
target_link_libraries(DICE10 ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build DICE10. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(DICE10_SCRIPTS
    vis.mac
    dice.mac
    ExampleSortFiles/DiceSort.C
    ExampleSortFiles/QuickPairsSort.C
    FieldExample.txt
	AutoDiceSort/AutoDice.sh
	AutoDiceSort/autovis.mac
	AutoDiceSort/ExptEquivSort.C
	AutoDiceSort/DiceEffScanCombinedSort.C
	AutoDiceSort/EffOnly.C
	AutoDiceSort/QuickAutoEff.sh
	AutoDiceSort/RawSort.C
  )
  

foreach(_script ${DICE10_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DICE10 DESTINATION bin)

