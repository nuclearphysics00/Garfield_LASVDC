g++ main.cpp DetectorSetup.cc SignalSimulation.cc IonizationCalculator.cc \
  `root-config --cflags --glibs` \
  -I$GARFIELD_HOME/include \
  -L$GARFIELD_HOME/lib64 -lGarfield \
  -o run_sim

