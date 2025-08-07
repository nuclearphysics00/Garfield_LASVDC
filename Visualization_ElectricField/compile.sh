g++ vdc.cpp \
  `root-config --cflags --glibs` \
  -I$GARFIELD_HOME/include \
  -L$GARFIELD_HOME/lib64 -lGarfield \
  -o run_sim

