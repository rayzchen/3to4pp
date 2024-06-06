git clone https://github.com/emscripten-core/emsdk
cd emsdk
./emsdk install latest
./emsdk activate latest
source emsdk_env.sh
cd ..
make emscripten
