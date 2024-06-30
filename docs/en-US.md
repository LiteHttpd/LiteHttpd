# LiteHttpd - A Lightweight, High-Performance Cross-Platform HTTP(S) Modular Service Framework
[LiteHttpd] is an HTTP service framework completely written in C++. With just the C++ standard library, you can easily [develop modules](#5-Module-Development) to handle HTTP requests.  
- LiteHttpd uses the [Lua] language to write [configuration files](#4-Configuration-Files) and supports [Lua 5.4] standard library functions, allowing you to quickly integrate new modules into the existing service system.
- LiteHttpd uses [libevent] to provide support for highly concurrent HTTP services.
- LiteHttpd uses [OpenSSL] to provide SSL protocol support.
- LiteHttpd uses [glog] for logging.
- LiteHttpd comes with an official [file server](#6-File-Server) module and supports [PHP] dynamic websites.

<toc/>

<div STYLE="page-break-after: always;"></div>

## 1 Installation
You can get the latest precompiled version of LiteHttpd from our [Release Page](https://github.com/LiteHttpd/LiteHttpd/releases/latest). Alternatively, you can obtain the latest LiteHttpd by [building it yourself](#2-Building-Yourself) from the latest code commits.  

> [!IMPORTANT]
> Running the MSVC version of LiteHttpd on the Windows platform requires the support of [Visual C++ Redistributable].  

> [!IMPORTANT]
> Running the MinGW version of LiteHttpd on the Windows platform requires the support of the MinGW runtime. You can obtain the MinGW runtime by installing the latest version of [MinGW].  

> [!IMPORTANT]
> Running LiteHttpd on the Linux platform requires the following dependencies (the package names below are for Ubuntu 24.04 LTS; please install the corresponding dependencies for other versions):  
> - libevent-core-2.1-7t64
> - libevent-extra-2.1-7t64
> - libevent-openssl-2.1-7t64
> - libevent-pthreads-2.1-7t64
> - openssl
> - liblua5.4-0
> - libgoogle-glog0v6t64

<div STYLE="page-break-after: always;"></div>

## 2 Building Yourself
### Getting the Source Code
You can get the latest source code from the `main` branch and initialize the [vcpkg] package manager with the following commands. These commands require [Git].  

#### Getting the Source Code on Windows
```
git clone https://github.com/LiteHttpd/LiteHttpd.git
cd LiteHttpd
git submodule update --init --recursive
cd vcpkg
.\bootstrap-vcpkg.bat
cd ..
```

#### Getting the Source Code on Linux
```
git clone https://github.com/LiteHttpd/LiteHttpd.git
cd LiteHttpd
git submodule update --init --recursive
cd vcpkg
./bootstrap-vcpkg.sh
cd ..
```

### Building the Binaries
The build process for LiteHttpd is based on [CMake] and [Ninja], so you need to ensure that they are correctly installed.  

#### Building with MSVC on Windows
Before starting, you need to confirm the MSVC installation directory and correctly set the `%VCINSTALLDIR%` environment variable. You can install [Visual Studio] with the latest version of MSVC from here.  

##### Building the Debug Version with MSVC
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-windows -B out/build/x64-Debug-MSVC .
ninja -C out/build/x64-Debug-MSVC -j 8 all
```

##### Building the Release Version with MSVC
```
"%VCINSTALLDIR%\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-windows -B out/build/x64-Release-MSVC .
ninja -C out/build/x64-Release-MSVC -j 8 all
```

#### Building with MinGW on Windows
Before starting, you need to install the latest version of [MinGW] and correctly set the environment variables. Additionally, this version still requires MSVC support. You can install [Visual Studio] with the latest version of MSVC from here.  

##### Building the Debug Version with MinGW
```
set CC=gcc
set CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-mingw-dynamic -B out/build/x64-Debug-MinGW .
ninja -C out/build/x64-Debug-MinGW -j 8 all
```

##### Building the Release Version with MinGW
```
set CC=gcc
set CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-mingw-dynamic -B out/build/x64-Release-MinGW .
ninja -C out/build/x64-Release-MinGW -j 8 all
```

#### Building with GCC on Linux
Before starting, you need to install the latest versions of the GCC and G++ compilers. On Ubuntu, you can get the compilers by installing the `build-essential` package.  

##### Building the Debug Version with GCC
```
export CC=gcc
export CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-linux -B ./out/build/x64-Debug-GCC .
ninja -C ./out/build/x64-Debug-GCC -j 8 all
```

##### Building the Release Version with GCC
```
export CC=gcc
export CXX=c++
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:STRING=./vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET:STRING=x64-linux -B ./out/build/x64-Release-GCC .
ninja -C ./out/build/x64-Release-GCC -j 8 all
```

<div STYLE="page-break-after: always;"></div>

## 3 Startup Parameters
Run LiteHttpd with the following command:  

```
LiteHttpd <configuration_file_path>
```

If the configuration file path is not specified, LiteHttpd will automatically use `config.lua` in the working directory as the default configuration file.  

<div STYLE="page-break-after: always;"></div>

## 4 Configuration File
The configuration file uses the [Lua] language, syntax version [Lua 5.4], and supports the Lua standard library.

### Function Documentation
#### set_port(port)
> **Parameters:**  
> - port: **Integer**, the port number which the current server process will listen to. The default port is 80.
>
> **Returns:** None.  

#### set_https(use_https)
> **Parameters:**  
> - use_https: **Boolean**, whether the current server uses HTTPS. The default is not to use HTTPS.
>
> **Returns:** None.  

#### set_default_page(path)
> **Parameters:**  
> - path: **String**, the path to the server's welcome page. The default is `default.html` in the LiteHttpd directory.
>
> **Returns:** None.  

#### add_module(host, path)
> **Parameters:**  
> - host: **String**, the host name in the HTTP request.
> - path: **String**, the path to the module handling the corresponding request. No need to add file extension; LiteHttpd will append `.dll`, `.so`, or `.dylib` based on the platform.
>
> **Returns:** None.  

#### remove_module(host)
> **Parameters:**  
> - host: **String**, the host name in the HTTP request.
>
> **Returns:** None.  

#### add_cer(host, key_path, cer_path)
> **Parameters:**  
> - host: **String**, the host name in the SNI field during SSL handshake.
> - key_path: **String**, the path to the SSL private key. The SSL private key should be in PEM format.
> - cer_path: **String**, the path to the SSL certificate. The SSL certificate should be a PEM format X.509 certificate chain, with the CA root certificate and intermediate certificates concatenated in order after the site certificate to form the certificate chain.
>
> **Returns:** None.  

#### remove_cer(host)
> **Parameters:**  
> - host: **String**, the host name in the SNI field during SSL handshake.
>
> **Returns:** None.  

### Example Configuration File
Below is an example of the default configuration file:  

```lua
set_port(443) -- Server listens on port 443
set_https(true) -- Use HTTPS service

-- Set SSL private key and certificate for the localhost domain
add_cer("localhost", "./localhost.key", "./localhost.cer")
-- Specify using the LiteHttpd.FileServer module to handle HTTP requests for localhost
add_module("localhost", "./LiteHttpd.FileServer")
```

### Example of Default Welcome Page
![Default Welcome Page](welcome-page.jpeg)

<div STYLE="page-break-after: always;"></div>

## 5 Module Development
LiteHttpd allows for convenient development of modules to handle different HTTP requests and provides the [LiteHttpd SDK] to support module development.  

### Importing the LiteHttpd SDK
You can add the LiteHttpd SDK as a submodule to your project using [Git]:  

```
git submodule add https://github.com/LiteHttpd/LiteHttpd.Dev.git
```

When using [CMake], you can link the LiteHttpd SDK with the following commands:  

```cmake
# Import the SDK
if (NOT TARGET LiteHttpdDev::core)
    add_subdirectory(LiteHttpd.Dev)
endif()

# Link the SDK
target_link_libraries(your_target PRIVATE LiteHttpdDev::core)
```

### Including SDK Header Files in Your Code
Include the `LiteHttpdDev.h` header file in the files where you need to use the SDK:  

```cpp
#include <LiteHttpdDev.h>
```

### Writing a Module Class
Create your own module class and inherit it from the `ModuleBase` class. Override the `processRequest` method to implement your own request handling codes:  

```cpp
class YourModule : public ModuleBase {

    // ...

public:
	void processRequest(const RequestParams& rp) override;

    // ...

};

void YourModule::processRequest(const RequestParams& rp) {
    // Handling Request
}
```
 
### Registering the Module Class
Use the `LITEHTTPD_MODULE` macro to register your module class as the exported module class:  

```cpp
LITEHTTPD_MODULE(YourModule)
```

### Request Parameters Reference
The `processRequest` method takes a parameter of type `RequestParams`, which has the following properties and public methods:  
- **protocol: ProtocolType** The protocol type of the request
- **addr: string** The host field of the request
- **port: uint16_t** The port number of the request
- **path: string** The path field of the request
- **query: string** The query field of the request
- **params: ParamList** The parsed parameters from the request query
- **method: MethodType** The HTTP method of the request
- **headers: ParamList** The list of request headers
- **data: vector<char>** The content of the request data
- **peerAddr: string** The IP address of the client
- **peerPort: uint16_t** The port number of the client

- **int getResponseCode()** Returns the HTTP status code if the request has been responded, otherwise returns 0
- **void reply(int code, const std::vector<char>& data)** Sends the response code and response data. This method must be called once per request, unless using chunked responses
- **void replyStart(int code)** Starts sending a chunked response. Each request must have either a regular response or a chunked response
- **void replyData(const std::vector<char>& data)** Sends chunked response data
- **void replyEnd()** Ends the chunked response
- **void addHeader(const std::string& key, const std::string& value)** Adds a header field to the response for the current request
- **void log(LogLevel level, const std::string& data)** Writes a log message
- **const FPMResult callFPM(const std::string& addr, uint16_t port, const std::vector<char>& data, const ParamList& params)** Calls PHP and receives the result

<div STYLE="page-break-after: always;"></div>

## 6 File Server
[LiteHttpd.FileServer] is the official implementation of the LiteHttpd file server module.  
This module communicates with PHP-FPM via FastCGI to execute PHP script files. This feature requires installing [PHP] and starting the FPM or CGI-related services.  
It can also communicate with other services via FastCGI.  

### File Server Configuration File
LiteHttpd.FileServer uses JSON as the configuration file format, located in the module directory as `LiteHttpd.FileServer.json`.  
Below is an example of the default configuration file:  

```json
{
  "survival": 60,
  "root": "C:/wwwroot/$hostname$",
  "page404": "404.html",
  "page403": "403.html",
  "defaultPage": "index.html",
  "fpm": {
    "surfix": ".php",
    "address": "127.0.0.1",
    "port": 9000,
    "fcgi_children": 2,
    "fcgi_max_requests": 1000
  }
}
```

- **survival:** 文件缓存生存时长，单位为秒，默认值为 `60`
- **root:** 网站根目录位置，支持匹配替换 `$hostname$` `$port$` 为实际参数，默认值为 `"./$hostname$"`
- **page404:** 404 错误页面路径，支持匹配替换 `$hostname$` `$port$` `$root$` 为实际参数，默认值为 `"404.html"`
- **page403:** 403 错误页面路径，支持匹配替换参数同 404，默认值为 `"403.html"`
- **defaultPage:** 网站默认页面名称，当请求 path 为路径时将使用路径下默认页面返回，默认值为 `"index.html"`
- **fpm.surfix:** 要通过 FastCGI 处理的文件类型，默认为 `".php"`
- **fpm.address:** FastCGI 服务地址，默认为 `"127.0.0.1"`
- **fpm.port:** FastCGI 服务端口，默认为 `9000`
- **fpm.fcgi_children:** 传递给 PHP-FPM 的 `PHP_FCGI_CHILDREN` 参数，默认为 `2`
- **fpm.fcgi_max_requests:** 传递给 PHP-FPM 的 `PHP_FCGI_MAX_REQUESTS` 参数，默认为 `1000`
- **survival:** The lifespan of the file cache in seconds. The default value is `60`.
- **root:** The location of the website's root directory. Supports matching and replacing `$hostname$` and `$port$` with actual parameters. The default value is `"./$hostname$"`.
- **page404:** The path to the 404 error page. Supports matching and replacing `$hostname$`, `$port$`, and `$root$` with actual parameters. The default value is `"404.html"`.
- **page403:** The path to the 403 error page. Supports the same parameter matching as the 404 error page. The default value is `"403.html"`.
- **defaultPage:** The default page name for the website. When the request path is a directory, the default page under the directory will be returned. The default value is `"index.html"`.
- **fpm.surfix:** The file types to be processed via FastCGI. The default value is `".php"`.
- **fpm.address:** The FastCGI service address. The default value is `"127.0.0.1"`.
- **fpm.port:** The FastCGI service port. The default value is `9000`.
- **fpm.fcgi_children:** The `PHP_FCGI_CHILDREN` parameter passed to PHP-FPM. The default value is `2`.
- **fpm.fcgi_max_requests:** The `PHP_FCGI_MAX_REQUESTS` parameter passed to PHP-FPM. The default value is `1000`.

### Default 403 Page Example
![Default 403 Page](403-page.jpeg)

### Default 404 Page Example
![Default 404 Page](404-page.jpeg)

### PHP Page Example
![PHP Info Page](php-info.jpeg)

<div STYLE="page-break-after: always;"></div>

## 7 日志系统
LiteHttpd 使用 [glog] 记录应用日志，并将日志按照 `INFO` `WARNING` `ERROR` `FATAL` 四个等级记录在 LiteHttpd 安装目录下 `logs` 目录中。当 LiteHttpd 使用 `Debug` 选项构建时，`DEBUG` 等级日志将与 `INFO` 等级一同记录，并在消息前添加 `(Debug)` 提示。  

### 日志命名
一条标准的 LiteHttpd 日志文件命名如下：  

```
LiteHttpd.HOSTNAME.USERNAME.log.LEVEL.yyyymmdd-hhmmss.uuuuuu
```

- **HOSTNAME:** 当前主机名称
- **USERNAME:** 当前用户名称
- **LEVEL:** 日志等级
- **yyyymmdd:** 进程启动日期，年月日
- **hhmmss:** 进程启动时间，时分秒
- **uuuuuu:** 进程启动时间，微秒

### 日志格式
一条标准的 LiteHttpd 日志格式如下：  

```
[IWEF yyyymmdd hh:mm:ss.uuuuuu threadid] msg
```

- **IWEF:** 日志等级，以单个字母表示
- **yyyymmdd:** 年月日
- **hh:mm:ss.uuuuuu:** 时间，精确至微秒
- **threadid:** 产生日志的线程 ID
- **msg:** 日志内容

<div STYLE="page-break-after: always;"></div>

## 8 架构设计
LiteHttpd 架构分为四个层次：  

```mermaid
block-beta
    columns 8

    m1["模块"]:2 m2["模块"]:2 m3["模块"]:2 m4["模块"]:2

    block:litehttpd:6
        http fpm ssl config module log
    end
    sdk["LiteHttpd SDK"]:2

    libevent:2 OpenSSL:2 Lua:2 glog:2

    system["操作系统接口"]:8
```

操作系统接口层之上为依赖层，依赖层为 http、SSL、Lua、日志四个子系统提供支持。  
依赖层之上为服务层，服务层由 LiteHttpd 程序与 SDK 共同组成。其中 LiteHttpd 分为 http、fpm、ssl、config、module、log 六大模块。  
服务层之上为模块层，由一个或多个请求处理模块构成。模块通过 SDK 与 LiteHttpd 本体交互，实现对不同 http 请求的响应。  

<div STYLE="page-break-after: always;"></div>

## 9 CI/CD 流水线
LiteHttpd 使用 [GitHub Actions] 搭建 CI/CD 流水线，该流水线分为 [构建](https://github.com/LiteHttpd/LiteHttpd/actions/workflows/build-artifacts.yml) 与 [发布](https://github.com/LiteHttpd/LiteHttpd/actions/workflows/upload-release.yml) 两部分。  

### 构建流水线
当 [仓库主分支](https://github.com/LiteHttpd/LiteHttpd/tree/main) 产生新的推送（push）到远程仓库或产生拉取请求（PR）时，将会触发构建流水线。构建流水线分为以下的步骤：  
1. 在 Windows 和 Linux 平台上分别进行 vcpkg 包的预构建并产生包缓存
2. 在 Windows 和 Linux 平台上分别构建 LiteHttpd 程序及官方模块
3. 清理构建过程中产生的临时文件
4. 对 LiteHttpd 程序及官方模块进行 [Sigstore] 签名
5. 对构建结果打包储存为构件（Artifacts）

![构建流水线](ci-build.jpeg)

### 发布流水线
当仓库产生新的标签（tag）时，将会触发发布流水线。发布流水线分为以下步骤：  
1. 执行构建流水线
2. 创建新的发行版（Release）
3. 下载构建流水线的构建结果构件
4. 重新打包构建结果为适应相应平台的格式
5. 上传包到发行版中

![发布流水线](ci-release.jpeg)

<div STYLE="page-break-after: always;"></div>

# 10 相关链接
- [Lua]: The Programming Language Lua.
- [MinGW]: MinGW-W64 compiler binaries.
- [vcpkg]: Open source C/C++ dependency manager from Microsoft
- [Git]: A free and open source distributed version control system.
- [CMake]: A Powerful Software Build System.
- [Ninja]: A small build system with a focus on speed.
- [Visual Studio]: 面向软件开发人员和 Teams 的 IDE 和代码编辑器。
- [libevent]: The libevent API provides a mechanism to execute a callback function when a specific event occurs on a file descriptor or after a timeout has been reached.
- [OpenSSL]: Cryptography and SSL/TLS Toolkit.
- [glog]: Google Logging Library.
- [PHP]: A popular general-purpose scripting language that is especially suited to web development.
- [LiteHttpd SDK]: Module SDK for LiteHttpd.
- [LiteHttpd.FileServer]: A simple LiteHttpd file server module.
- [Sigstore]: sign. verify. protect.

[LiteHttpd]: <https://github.com/LiteHttpd/LiteHttpd>
[Lua]: <https://www.lua.org/>
[Lua 5.4]: <https://www.lua.org/manual/5.4/>
[MinGW]: <https://github.com/niXman/mingw-builds-binaries>
[Visual C++ 可再发行组件]: <https://aka.ms/vs/17/release/VC_redist.x64.exe>
[vcpkg]: <https://vcpkg.io/>
[Git]: <https://git-scm.com/>
[CMake]: <https://cmake.org/>
[Ninja]: <https://ninja-build.org/>
[Visual Studio]: <https://visualstudio.microsoft.com/>
[libevent]: <https://libevent.org/>
[OpenSSL]: <https://www.openssl.org/>
[glog]: <https://google.github.io/glog>
[PHP]: <https://www.php.net/>
[LiteHttpd SDK]: <https://github.com/LiteHttpd/LiteHttpd.Dev>
[LiteHttpd.FileServer]: <https://github.com/LiteHttpd/LiteHttpd.FileServer>
[GitHub Actions]: <https://github.com/LiteHttpd/LiteHttpd/actions>
[Sigstore]: <https://www.sigstore.dev/>
