cmake_minimum_required (VERSION 3.15)

project (LiteHttpd VERSION 1.0.0 LANGUAGES CXX C)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release"  CACHE STRING "Choose the type of build." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_EXTENSIONS OFF)
set (CMAKE_C_STANDARD_REQUIRED ON)

set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if (MSVC)
    add_compile_definitions ("_CRT_SECURE_NO_WARNINGS")
endif (MSVC)

if (WIN32)
	set (COMPILE_SYS_DEF "WIN32=1")
elseif (UNIX AND NOT APPLE)
	set (COMPILE_SYS_DEF "LINUX=1")
elseif (APPLE)
	set (COMPILE_SYS_DEF "DARWIN=1")
endif ()

# Binary Path
set (OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

# Packages
find_package (OpenSSL REQUIRED)
find_package (Libevent CONFIG REQUIRED)

# Source Files
file (GLOB_RECURSE LITEHTTPD_SRC CONFIGURE_DEPENDS "./source/*.cpp" "./source/*.c" "./source/*.rc")

# Target
add_executable (LiteHttpd ${LITEHTTPD_SRC})
target_include_directories (LiteHttpd PRIVATE "./source")
target_compile_definitions (LiteHttpd PRIVATE ${COMPILE_SYS_DEF})
target_compile_definitions (LiteHttpd PRIVATE
	"PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
	"PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
	"PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
)
target_link_libraries (LiteHttpd PRIVATE
	OpenSSL::SSL OpenSSL::Crypto
	libevent::core libevent::extra libevent::openssl
)

if (NOT MSVC)
	target_compile_options (LiteHttpd PRIVATE
		-pthread -fPIE)
endif (NOT MSVC)
if (WIN32)
    target_link_libraries (LiteHttpd PRIVATE Dbghelp)
endif (WIN32)
if (MSVC)
	target_link_options (LiteHttpd PRIVATE 
		/SUBSYSTEM:CONSOLE
	)
endif (MSVC)

set_target_properties (LiteHttpd PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")