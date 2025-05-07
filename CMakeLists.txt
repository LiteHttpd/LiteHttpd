cmake_minimum_required (VERSION 3.15)

project (LiteHttpd VERSION 1.1.2 LANGUAGES CXX C)

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
find_package (Lua REQUIRED)
find_package (glog CONFIG REQUIRED)

# Get SDK
add_subdirectory (LiteHttpd.Dev)

# Get LiteHttpd.FileServer
add_subdirectory (LiteHttpd.FileServer)

# Get FastCGI
add_subdirectory (FastCGI)

# OpenSSL Debug
find_program(OPENSSL_EXECUTABLE NAMES openssl)
message (STATUS "OpenSSL excutable path: ${OPENSSL_EXECUTABLE}")
execute_process (
	COMMAND ${OPENSSL_EXECUTABLE} --version
	OUTPUT_VARIABLE openssl_out
	ERROR_VARIABLE openssl_err
	OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_STRIP_TRAILING_WHITESPACE
	ERROR_QUIET
	RESULT_VARIABLE openssl_code
)

if (openssl_code EQUAL 0)
	message (STATUS "${openssl_out}")
else ()
	message (FATAL_ERROR "Error getting OpenSSL version with exit code ${openssl_code}: ${openssl_err}")
endif ()

# App Configs
file (GLOB_RECURSE APP_CONFIG_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config/*")
set (APP_CONFIG_OUTPUT "")
foreach (ITEM ${APP_CONFIG_SRC})
    string (REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/config" "${OUTPUT_DIR}" ITEM_TARGET ${ITEM})
	list (APPEND APP_CONFIG_OUTPUT ${ITEM_TARGET})

	cmake_path (CONVERT ${ITEM} TO_NATIVE_PATH_LIST ITEM_INPUT)
	cmake_path (CONVERT ${ITEM_TARGET} TO_NATIVE_PATH_LIST ITEM_OUTPUT)

	if (WIN32)
		add_custom_command (
			OUTPUT ${ITEM_TARGET}
			COMMAND copy ${ITEM_INPUT} ${ITEM_OUTPUT}
			DEPENDS ${ITEM}
			COMMENT "Copying application config file: ${ITEM}"
			VERBATIM)
	else ()
			add_custom_command (
			OUTPUT ${ITEM_TARGET}
			COMMAND cp ${ITEM_INPUT} ${ITEM_OUTPUT}
			DEPENDS ${ITEM}
			COMMENT "Copying application config file: ${ITEM}"
			VERBATIM)
	endif ()
endforeach (ITEM ${APP_CONFIG_SRC})
add_custom_target (app_config_copy
	DEPENDS ${APP_CONFIG_OUTPUT}
	COMMENT "Copy application configs"
	VERBATIM)

# Generate Localhost Certificate
set (LOCALHOST_CSR_OUT "${OUTPUT_DIR}/localhost.csr")
set (LOCALHOST_CER_OUT "${OUTPUT_DIR}/localhost.cer")
set (LOCALHOST_KEY_OUT "${OUTPUT_DIR}/localhost.key")
get_filename_component (OPENSSL_DIR ${OPENSSL_EXECUTABLE} DIRECTORY)
set (OPENSSL_CNF_DIR ${OPENSSL_DIR})
if (NOT MSVC)
	get_filename_component (OPENSSL_CNF_DIR ${OPENSSL_CNF_DIR} DIRECTORY)
	get_filename_component (OPENSSL_CNF_DIR ${OPENSSL_CNF_DIR} DIRECTORY)
	set (OPENSSL_CNF_DIR "${OPENSSL_CNF_DIR}/etc/ssl")
endif ()
get_filename_component (OPENSSL_NAME ${OPENSSL_EXECUTABLE} NAME)
set (OPENSSL_CNF "${OPENSSL_CNF_DIR}/openssl.cnf")

add_custom_command (
	OUTPUT "${LOCALHOST_CER_OUT}" "${LOCALHOST_KEY_OUT}" "${LOCALHOST_CSR_OUT}"
	COMMAND ${OPENSSL_NAME} genrsa -out ${LOCALHOST_KEY_OUT} 2048
    COMMAND ${OPENSSL_NAME} req -new -key ${LOCALHOST_KEY_OUT} -out ${LOCALHOST_CSR_OUT} -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=localhost" -config ${OPENSSL_CNF}
    COMMAND ${OPENSSL_NAME} x509 -req -days 358000 -in ${LOCALHOST_CSR_OUT} -signkey ${LOCALHOST_KEY_OUT} -out ${LOCALHOST_CER_OUT} -extensions v3_req -extfile ${OPENSSL_CNF}
    DEPENDS ${OPENSSL_EXECUTABLE}
	WORKING_DIRECTORY ${OPENSSL_DIR}
	COMMENT "Generate localhost certificate to ${LOCALHOST_CER_OUT} ${LOCALHOST_KEY_OUT} ${LOCALHOST_CSR_OUT}"
	VERBATIM)

add_custom_target (localhost_cer_generate
	DEPENDS "${LOCALHOST_CER_OUT}" "${LOCALHOST_KEY_OUT}" "${LOCALHOST_CSR_OUT}"
	COMMENT "Generate localhost certificate"
	VERBATIM)

# Source Files
file (GLOB_RECURSE LITEHTTPD_SRC CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.rc")

# Target
add_executable (LiteHttpd ${LITEHTTPD_SRC})
target_include_directories (LiteHttpd PRIVATE ${LUA_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/source")
target_compile_definitions (LiteHttpd PRIVATE ${COMPILE_SYS_DEF})
target_compile_definitions (LiteHttpd PRIVATE
	"PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}"
	"PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}"
	"PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}"
)
target_compile_definitions (LiteHttpd PRIVATE GLOG_NO_ABBREVIATED_SEVERITIES)
if (WIN32)
	target_compile_definitions (LiteHttpd PRIVATE "WIN32_LEAN_AND_MEAN")
endif ()
target_link_libraries (LiteHttpd PRIVATE
	LiteHttpdDev::core FastCGI::fastcgi
	OpenSSL::SSL OpenSSL::Crypto
	libevent::core libevent::extra libevent::openssl
	${LUA_LIBRARIES}
	glog::glog
)
if (NOT WIN32)
	target_link_libraries (LiteHttpd PRIVATE libevent::pthreads)
endif ()

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

add_dependencies (LiteHttpd
	app_config_copy localhost_cer_generate
	LiteHttpd.FileServer
)

set_target_properties (LiteHttpd PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")