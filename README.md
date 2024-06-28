# LiteHttpd
A super lightweight http(s) service framework. Supports Windows and Linux platform. 

## Get Source Code
### Windows (MSVC)
```
git clone https://github.com/LiteHttpd/LiteHttpd.git
cd LiteHttpd
git submodule update --init --recursive
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
```

### Linux
```
git clone https://github.com/LiteHttpd/LiteHttpd.git
cd LiteHttpd
git submodule update --init --recursive
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
```

## Compile
### Windows Debug (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-windows -B out/build/x64-Debug-MSVC .
ninja -C out/build/x64-Debug-MSVC -j 8 all
```

### Windows Release (MSVC)
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-windows -B out/build/x64-Release-MSVC .
ninja -C out/build/x64-Release-MSVC -j 8 all
```

### Windows Debug (MinGW)
```
set CC=gcc
set CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-mingw-dynamic -B out/build/x64-Debug-MinGW .
ninja -C out/build/x64-Debug-MSVC -j 8 all
```

### Windows Release (MinGW)
```
set CC=gcc
set CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-mingw-dynamic -B out/build/x64-Release-MinGW .
ninja -C out/build/x64-Release-MSVC -j 8 all
```

### Linux Debug (GCC)
```
export CC=gcc
export CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-linux -B ./out/build/x64-Debug-GCC .
ninja -C ./out/build/x64-Debug-GCC -j 8 all
```

### Linux Release (GCC)
```
export CC=gcc
export CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-linux -B ./out/build/x64-Release-GCC .
ninja -C ./out/build/x64-Release-GCC -j 8 all
```
