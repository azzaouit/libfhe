# libfhe
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.9)

project(fhe
    VERSION 0.0.0
    DESCRIPTION "Library for Fully Homomorphic Encryption"
    HOMEPAGE_URL "https://libfhe.org"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -Wextra -Werror -march=native")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fno-omit-frame-pointer -fsanitize=undefined,address")
endif()

# =========== Required Libraries
find_library(gmp gmp)
if(NOT gmp)
  message(FATAL_ERROR "Required library: GMP Not Found")
endif()

find_library(m m)
if(NOT m)
  message(FATAL_ERROR "Required library: libm Not Found")
endif()

# =========== Optional Dependencies
option(WITH_OPENMP "Enable OpenMP" OFF)
if(WITH_OPENMP)
    find_package(OpenMP REQUIRED)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

# =========== Main shared library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB src_files "src/*.c" "src/utils/*.c" "src/*.h")
file(GLOB public_headers "include/*.h")
add_library(${PROJECT_NAME} SHARED ${src_files})
if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    target_link_options(${PROJECT_NAME} BEFORE PUBLIC -fno-omit-frame-pointer -fsanitize=undefined PUBLIC -fsanitize=address)
endif()
target_link_libraries(${PROJECT_NAME} gmp m)
if(WITH_OPENMP)
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_C)
endif()
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    PUBLIC_HEADER "${public_headers}"
)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

# =========== Test Suite
enable_testing()
file(GLOB files "tests/*.c")
foreach(file ${files})
    cmake_path(GET file STEM filename)
    add_executable(${filename} ${file})
    set_target_properties(${filename} PROPERTIES COMPILE_FLAGS "-g -fno-omit-frame-pointer")
    set_target_properties(${filename} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
	if(CMAKE_BUILD_TYPE MATCHES DEBUG)
        target_link_options(${filename} BEFORE PUBLIC -fno-omit-frame-pointer -fsanitize=undefined PUBLIC -fsanitize=address)
	endif()
    target_link_libraries(${filename} ${PROJECT_NAME})
    add_test(${filename} tests/${filename})
endforeach()

# =========== Examples
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
	file(GLOB files "examples/*.c")
	foreach(file ${files})
		cmake_path(GET file STEM filename)
		add_executable(${filename} ${file})
		set_target_properties(${filename} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples")
		if(CMAKE_BUILD_TYPE MATCHES DEBUG)
            target_link_options(${filename} BEFORE PUBLIC -fno-omit-frame-pointer -fsanitize=undefined PUBLIC -fsanitize=address)
		endif()
		target_link_libraries(${filename} ${PROJECT_NAME})
	endforeach()
endif()

# =========== Release archives
set(ARCHIVE_NAME lib${CMAKE_PROJECT_NAME}-${PROJECT_VERSION})
add_custom_target(dist 
    COMMAND git archive --format=tar.gz -o ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tgz HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
