# SPDX-License-Identifier: LGPL-2.1-or-later

add_library(FreeCADApp SHARED)

# =========================================================
# 🔥 MSVC ABI 强制统一（关键）
# =========================================================
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

# =========================================================
# Compile definitions
# =========================================================
target_compile_definitions(FreeCADApp PRIVATE FCApp)

if(BUILD_TRACY_FRAME_PROFILER)
    target_compile_definitions(FreeCADApp PRIVATE BUILD_TRACY_FRAME_PROFILER)
endif()

if(CMAKE_BUILD_TYPE)
    target_compile_definitions(FreeCADApp PRIVATE
        CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    )
endif()

if(FREECAD_RELEASE_SEH)
    target_compile_definitions(FreeCADApp PRIVATE HAVE_SEH)
endif()

if(RESOURCEDIR)
    target_compile_definitions(FreeCADApp PRIVATE RESOURCEDIR="${RESOURCEDIR}")
endif()

if(LIBRARYDIR)
    target_compile_definitions(FreeCADApp PRIVATE LIBRARYDIR="${LIBRARYDIR}")
endif()

if(DOCDIR)
    target_compile_definitions(FreeCADApp PRIVATE DOCDIR="${DOCDIR}")
endif()

# =========================================================
# Include dirs
# =========================================================
target_include_directories(FreeCADApp PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# =========================================================
# Qt6（必须统一 Qt6_DIR / toolchain）
# =========================================================
find_package(Qt6 REQUIRED COMPONENTS Core Xml)

target_link_libraries(FreeCADApp PRIVATE
    Qt6::Core
    Qt6::Xml
)

# =========================================================
# 🔥 BOOST（关键修复：必须 CONFIG + vcpkg）
# =========================================================
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED CONFIG COMPONENTS program_options)

target_link_libraries(FreeCADApp PRIVATE
    Boost::program_options
)

# =========================================================
# Core deps
# =========================================================
target_link_libraries(FreeCADApp PRIVATE
    FreeCADBase
    fmt::fmt
)

if(BUILD_TRACY_FRAME_PROFILER)
    target_link_libraries(FreeCADApp PRIVATE TracyClient)
endif()

# =========================================================
# Python generated headers (修复 CMakeScript 风险)
# =========================================================
set(_vars "const char CMakeVariables[] = R\"cmake(")
set(_delim "")

get_cmake_property(_variableNames VARIABLES)
foreach(_variableName ${_variableNames})
    if(_variableName MATCHES "^BUILD")
        set(_vars "${_vars}${_delim}\\n\"${_variableName}\"")
        set(_delim ",")
    endif()
endforeach()

set(_vars "${_vars}\n)cmake\";")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/CMakeScript.hh "${_vars}")

fc_copy_file_if_different(
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeScript.hh"
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeScript.h"
)

# =========================================================
# Sources
# =========================================================
target_sources(FreeCADApp PRIVATE
    ${FreeCADApp_SRCS}
)

# =========================================================
# PCH
# =========================================================
if(FREECAD_USE_PCH)
    target_precompile_headers(FreeCADApp PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/PreCompiled.h"
    )
endif()

# =========================================================
# MSVC warnings
# =========================================================
if(MSVC)
    target_compile_definitions(FreeCADApp PRIVATE WIN32_LEAN_AND_MEAN)
    target_compile_options(FreeCADApp PRIVATE /wd4251 /wd4275 /wd4273)
endif()

# =========================================================
# 强制链接顺序稳定（避免 LNK2019 偶发）
# =========================================================
set_target_properties(FreeCADApp PROPERTIES
    LINK_DEPENDS_NO_SHARED ON
)

# =========================================================
# Dependency
# =========================================================
add_dependencies(FreeCADApp fc_version)

# =========================================================
# Install
# =========================================================
set_bin_dir(FreeCADApp FreeCADApp)

if(WIN32)
    install(TARGETS FreeCADApp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    install(TARGETS FreeCADApp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()
