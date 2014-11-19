autoreconf -i
./configure CXXFLAGS=-I/usr/local/include\ $(pkg-config --cflags jsoncpp) LDFLAGS=-L/usr/local/lib\ -lcurlpp\ -lcurl\ -lboost_program_options\ -lboost_filesystem\ -lboost_system\ -ljsoncpp
curl -o src/cpptoml.h https://raw.githubusercontent.com/skystrife/cpptoml/master/include/cpptoml.h
make
