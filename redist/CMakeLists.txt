#
# Strelka - Small Variant Caller
# Copyright (c) 2009-2016 Illumina, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

################################################################################
##
## Configuration file for external tools
##
## author Come Raczy
##
################################################################################

message(STATUS "Building external tools")

include ("${THIS_GLOBALS_CMAKE}")
include ("${THIS_MACROS_CMAKE}")

# convenience macro to set in both current and parent scope:
macro(superset symbol value)
    set(${symbol} "${value}")
    set(${symbol} "${value}" PARENT_SCOPE)
endmacro()


set (THIS_CMAKE_PLATFORM "")
if (WIN32)
    # Sometimes the platform (x64,win32...) is specified as part of the target name
    # this captures the cases where it is specified separately:
    if (CMAKE_GENERATOR_PLATFORM)
        set (THIS_CMAKE_PLATFORM -A "${CMAKE_GENERATOR_PLATFORM}")
    endif ()
endif ()


#
# htslib
#
set(HTSLIB_PREFIX "htslib-1.2.1-204-g8197cfd")
superset(HTSLIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/${HTSLIB_PREFIX}")
superset(HTSLIB_LIBRARY "${HTSLIB_DIR}/libhts.a")

add_custom_command(
    OUTPUT ${HTSLIB_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${HTSLIB_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar xjf "${THIS_REDIST_DIR}/${HTSLIB_PREFIX}.tar.bz2"
    COMMENT "Unpacking htslib library")

set (HTSLIB_FINAL_TASK ${HTSLIB_DIR})

if (NOT WIN32)
    add_custom_command(
        OUTPUT ${HTSLIB_LIBRARY}
        COMMAND $(MAKE) lib-static bgzip htsfile tabix >htslib.build.log 2>htslib.build.error.log
        WORKING_DIRECTORY ${HTSLIB_DIR}
        DEPENDS ${HTSLIB_DIR}
        COMMENT "Building htslib library")

    set (HTSLIB_FINAL_TASK ${HTSLIB_LIBRARY})
endif ()

set(THIS_HTSLIB "${THIS_PROJECT_NAME}_htslib")
add_custom_target(${THIS_HTSLIB} DEPENDS "${HTSLIB_FINAL_TASK}")


if (NOT WIN32)
    install(PROGRAMS "${HTSLIB_DIR}/bgzip" DESTINATION "${THIS_LIBEXECDIR}")
    install(PROGRAMS "${HTSLIB_DIR}/htsfile" DESTINATION "${THIS_LIBEXECDIR}")
    install(PROGRAMS "${HTSLIB_DIR}/tabix" DESTINATION "${THIS_LIBEXECDIR}")
endif ()

#
# samtools
#
set(SAMTOOLS_PREFIX "samtools-1.2")
set(SAMTOOLS_DIR "${CMAKE_CURRENT_BINARY_DIR}/${SAMTOOLS_PREFIX}")
set(SAMTOOLS_LIBRARY "${SAMTOOLS_DIR}/libbam.a")
superset(SAMTOOLS_PROG "${SAMTOOLS_DIR}/samtools")

# final directory copy below would ideally be a soft-link, copy is for windows build
add_custom_command(
    OUTPUT ${SAMTOOLS_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${SAMTOOLS_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar xjf "${THIS_REDIST_DIR}/${SAMTOOLS_PREFIX}.tar.bz2"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${HTSLIB_DIR}" "${SAMTOOLS_DIR}/${HTSLIB_PREFIX}"
    DEPENDS ${HTSLIB_FINAL_TASK}
    COMMENT "Unpacking samtools package")

set (SAMTOOLS_FINAL_TASK ${SAMTOOLS_DIR})

if (NOT WIN32)
    add_custom_command(
        OUTPUT ${SAMTOOLS_PROG}
        COMMAND $(MAKE) HTSDIR=${HTSLIB_PREFIX} all 2>| samtools.build.log
        DEPENDS ${HTSLIB_LIBRARY}
        DEPENDS ${SAMTOOLS_DIR}
        WORKING_DIRECTORY ${SAMTOOLS_DIR}
        COMMENT "Building samtools package")

    set (SAMTOOLS_FINAL_TASK ${SAMTOOLS_PROG})
endif ()

set(THIS_SAMTOOLS "${THIS_PROJECT_NAME}_samtools")
add_custom_target(${THIS_SAMTOOLS} DEPENDS "${SAMTOOLS_FINAL_TASK}")

if (NOT WIN32)
    install(PROGRAMS "${SAMTOOLS_PROG}" DESTINATION "${THIS_LIBEXECDIR}")
endif ()

if (NOT WIN32)
    #
    # bgzf_extras
    #
    set(BGZFX_PREFIX "bgzf_extras-v0.1")
    superset(BGZFX_DIR "${CMAKE_CURRENT_BINARY_DIR}/${BGZFX_PREFIX}")
    set(BGCAT_PROG "${BGZFX_DIR}/bgzf_cat")
    set(BGZIP9_PROG "${BGZFX_DIR}/bgzip9")

    add_custom_command(
        OUTPUT ${BGCAT_PROG}
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${BGZFX_DIR}"
        COMMAND ${CMAKE_COMMAND} -E tar xjf "${THIS_REDIST_DIR}/${BGZFX_PREFIX}.tar.bz2"
        COMMAND $(MAKE) -C "${BGZFX_DIR}" all 2>| bgzf_extras.build.log
        COMMENT "Building bgzf_extras")

    set(THIS_BGZFX "${THIS_PROJECT_NAME}_bgzfx")
    add_custom_target(${THIS_BGZFX} DEPENDS "${BGCAT_PROG}")

    install(PROGRAMS "${BGCAT_PROG}" DESTINATION "${THIS_LIBEXECDIR}")
    install(PROGRAMS "${BGZIP9_PROG}" DESTINATION "${THIS_LIBEXECDIR}")
endif ()



#
# jsoncpp
#
set(JSONCPP_PREFIX "jsoncpp-0.10.4")
set(JSONCPP_DIR "${CMAKE_CURRENT_BINARY_DIR}/${JSONCPP_PREFIX}")
set(JSONCPP_BUILD_DIR "${JSONCPP_DIR}/build")
set(JSONCPP_INSTALL_DIR "${JSONCPP_DIR}/install")
superset(JSONCPP_INCLUDE_DIR "${JSONCPP_INSTALL_DIR}/include")
superset(JSONCPP_LIBRARY "${JSONCPP_INSTALL_DIR}/lib/libjsoncpp.a")

add_custom_command(
    OUTPUT ${JSONCPP_BUILD_DIR}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${JSONCPP_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar xjf "${THIS_REDIST_DIR}/${JSONCPP_PREFIX}.tar.bz2"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${JSONCPP_BUILD_DIR}"
    COMMENT "Unpacking jsoncpp package")

set (JSONCPP_FINAL_TASK ${JSONCPP_DIR})

if (WIN32)
    # fake the header installation for win32 until we add real build support:
    add_custom_command(
        OUTPUT "${JSONCPP_INCLUDE_DIR}/json"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${JSONCPP_INCLUDE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${JSONCPP_DIR}/include" "${JSONCPP_INCLUDE_DIR}"
        DEPENDS ${JSONCPP_BUILD_DIR}
        WORKING_DIRECTORY ${JSONCPP_BUILD_DIR}
        COMMENT "Installing JsonCpp headers")

    set (JSONCPP_FINAL_TASK "${JSONCPP_INCLUDE_DIR}/json")
else ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set (JSONCPP_CMAKE_CXX_FLAGS "-DCMAKE_CXX_FLAGS=-D_GLIBCXX_USE_CXX11_ABI=0")
    endif ()
    add_custom_command(
        OUTPUT ${JSONCPP_LIBRARY}
        COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX:PATH=${JSONCPP_INSTALL_DIR} -DCMAKE_C_COMPILER="${CMAKE_C_COMPILER}" -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER}" ${JSONCPP_CMAKE_CXX_FLAGS} -G "Unix Makefiles" ${JSONCPP_DIR} > jsoncpp.config.log

        COMMAND $(MAKE) install > jsoncpp.build.log
        DEPENDS ${JSONCPP_BUILD_DIR}
        WORKING_DIRECTORY ${JSONCPP_BUILD_DIR}
        COMMENT "Building JsonCpp Library")

    set (JSONCPP_FINAL_TASK ${JSONCPP_LIBRARY})
endif ()

set(THIS_JSONCPP "${THIS_PROJECT_NAME}_jsoncpp")
add_custom_target(${THIS_JSONCPP} DEPENDS "${JSONCPP_FINAL_TASK}")



#
# codemin
#
set(CODEMIN_PREFIX "CodeMin-1.0.5")
superset(CODEMIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CODEMIN_PREFIX}/include")
set(CODEMIN_MARK "${CODEMIN_DIR}/minimize_1d.h")

add_custom_command(
    OUTPUT ${CODEMIN_MARK}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CODEMIN_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar xjf "${THIS_REDIST_DIR}/${CODEMIN_PREFIX}.tar.bz2"
    COMMENT "Unpacking CodeMin")

set(THIS_CODEMIN "${THIS_PROJECT_NAME}_codemin")
add_custom_target(${THIS_CODEMIN} DEPENDS "${CODEMIN_MARK}")

#
# pyflow
#

set(PYFLOW_PREFIX "pyflow-1.1.13")
set(PYFLOW_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PYFLOW_PREFIX}")
set(PYFLOW_SCRIPT "${PYFLOW_DIR}/src/pyflow.py")

set (PYFLOW_DEPENDS "${PYFLOW_SCRIPT}")
add_custom_command(
    OUTPUT ${PYFLOW_SCRIPT}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${PYFLOW_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar xjf "${THIS_REDIST_DIR}/${PYFLOW_PREFIX}.tar.bz2"
    COMMAND ${CMAKE_COMMAND} -E remove -f "${PYFLOW_DIR}/src/__init__.py"
    COMMENT "Unpacking pyflow")

if (PYTHONINTERP_FOUND)
    set (PYFLOW_DEPENDS "${PYFLOW_SCRIPT}c")
    add_custom_command(
        OUTPUT ${PYFLOW_SCRIPT}c
        COMMAND ${PYTHON_EXECUTABLE} -m compileall -q "${PYFLOW_DIR}/src"
        DEPENDS ${PYFLOW_SCRIPT}
        COMMENT "Building pyflow")
endif()

install_python_lib_dir("${PYFLOW_DIR}/src" "${THIS_PYTHON_LIBDIR}/pyflow")

set (THIS_PYFLOW "${THIS_PROJECT_NAME}_pyflow")
add_custom_target(${THIS_PYFLOW} DEPENDS "${PYFLOW_DEPENDS}")


# tie results back to parent:
#
add_dependencies(${THIS_OPT} ${THIS_SAMTOOLS} ${THIS_JSONCPP} ${THIS_CODEMIN} ${THIS_PYFLOW})

if (NOT WIN32)
    add_dependencies(${THIS_OPT} ${THIS_BGZFX})
endif ()

