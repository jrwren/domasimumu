This was an experiment for me to play with modern C++. I hit too much friction.
I quit before I finished.

pushd src
curl -O https://raw.githubusercontent.com/skystrife/cpptoml/master/include/cpptoml.h
curl -O https://raw.githubusercontent.com/matiu2/curlpp11/master/src/curlpp11.hpp
curl -O https://raw.githubusercontent.com/matiu2/curlpp11/master/src/curlpp11.cpp
popd
autoreconf -i
./configure CXXFLAGS=-I/usr/local/include\ $(pkg-config --cflags jsoncpp) LDFLAGS=-L/usr/local/lib\ -lcurl\ -lboost_program_options\ -lboost_filesystem\ -lboost_system\ -ljsoncpp
make

TODO: write something to read output of

    nm -an -f sysv src/domasimumu.o

and show size of each entry, then maybe group sizes by similar names to see which templates are using all that space.

Setup lint and cppcheck.
