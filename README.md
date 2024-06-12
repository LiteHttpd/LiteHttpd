# LiteHttpd
A super lightweight HTTP service framework.
> [!IMPORTANT]
> Under development, not released.  

## Get Source Code
### Windows (MSVC)
```
git clone https://github.com/LiteHttpd/LiteHttpd.git
cd LiteHttpd
git submodule update --init --recursive
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
vcpkg\vcpkg install 
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
