# DICEgeant4

Geant4 simulation of DICE electron spectrometer, based on GRSI GRIFFIN code.
Test on version geant4.10.06.p03

DICE10.CC has been configured to default for remote work running the program with no arguments:
  ./DICE
will enter the tsch non-GUI geant4 command prompt and execute the commands of remote_vis.mac

Running with the -l argument:
  ./DICE -l
Will open the full qt/OpenGL GUI and execute the commands of vis.mac

To run in batch mode with no GUI or visualization run:
  ./DICE MACRO_FILE.mac
An output TTree will be saved to g4out.root
An example macro dice.mac is provided
A simple root sort script DiceSort.C is provided

