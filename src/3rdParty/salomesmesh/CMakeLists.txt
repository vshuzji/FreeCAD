cmake_minimum_required(VERSION 3.20)
project(SalomeSMESH LANGUAGES C CXX)

# =========================
# 基础设置
# =========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC OFF CACHE INTERNAL "Disable Qt AUTOMOC for salomesmesh")

set(SMESH_VERSION_MAJOR 7)
set(SMESH_VERSION_MINOR 7)
set(SMESH_VERSION_PATCH 1)
set(SMESH_VERSION_TWEAK 0)

# =========================
# 编译器警告抑制
# =========================
if(MSVC)
    add_compile_options(/wd4018 /wd4244 /wd4267 /wd4251 /wd4996)
elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    add_compile_options(
        -Wno-sign-compare
        -Wno-reorder
        -Wno-unused-variable
        -Wno-unused-parameter
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
    add_compile_options(
        -Wno-sign-compare
        -Wno-reorder
        -Wno-unused-variable
        -Wno-unused-parameter
    )
endif()

# =========================
# VTK（vcpkg正确方式）
# =========================
find_package(VTK CONFIG REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    CommonSystem
    FiltersCore
    RenderingCore
    RenderingOpenGL2
)

# =========================
# OpenCASCADE
# =========================
set(OCC_ROOT "G:/OCCT/INSTALL")

set(OCC_LIBS
    ${OCC_ROOT}/lib/TKernel.lib
    ${OCC_ROOT}/lib/TKMath.lib
    ${OCC_ROOT}/lib/TKBRep.lib
    ${OCC_ROOT}/lib/TKTopAlgo.lib
    ${OCC_ROOT}/lib/TKGeomBase.lib
    ${OCC_ROOT}/lib/TKGeomAlgo.lib
)

# =========================
# include
# =========================
set(SMESH_INCLUDES
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}

    ${OCC_ROOT}/include
    ${MED_ROOT}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# =========================
# SMESH公共库集合
# =========================

# =========================
# MED
# =========================
set(MED_ROOT "G:/MEDFileOuput")

# =========================
# Boost（vcpkg 自动查找）
# =========================

# 强制使用你现有的 medC.lib，无论 Debug / Release
set(MED_LIBS
    medC.lib
    medimport.lib
)

set(SMESH_LIBS
    Rpcrt4.lib
    ${MED_LIBS}
    ${OCC_LIBS}
)

# =========================
# 平台定义
# =========================
if(WIN32)
    add_definitions(-DWNT -DWIN32 -D_WINDOWS -DCSFDB)
    add_definitions(-D_OCC64)
endif()

# =========================================================
# SMDS
# =========================================================
file(GLOB SMDS_SRC src/SMDS/*.cpp)
add_library(SMDS SHARED ${SMDS_SRC})

target_include_directories(SMDS PRIVATE ${SMESH_INCLUDES})
target_link_libraries(SMDS PRIVATE
    ${SMESH_LIBS}
    VTK::CommonCore
)

# =========================================================
# Driver
# =========================================================
file(GLOB Driver_SRC src/Driver/*.cpp)
add_library(Driver SHARED ${Driver_SRC})

target_include_directories(Driver PRIVATE ${SMESH_INCLUDES})
target_link_libraries(Driver PRIVATE ${SMESH_LIBS})

# =========================================================
# DriverSTL
# =========================================================
file(GLOB DriverSTL_SRC src/DriverSTL/*.cpp)
add_library(DriverSTL SHARED ${DriverSTL_SRC})

target_include_directories(DriverSTL PRIVATE ${SMESH_INCLUDES} ${MED_ROOT}/lib)

target_link_libraries(DriverSTL PRIVATE
    Driver
    SMDS
    ${SMESH_LIBS}
    VTK::CommonCore
)

# =========================================================
# DriverDAT
# =========================================================
file(GLOB DriverDAT_SRC src/DriverDAT/*.cpp)
add_library(DriverDAT SHARED ${DriverDAT_SRC})

target_include_directories(DriverDAT PRIVATE ${SMESH_INCLUDES})

target_link_libraries(DriverDAT PRIVATE
    Driver
    SMDS
    ${SMESH_LIBS}
)

# =========================================================
# SMESHDS（ ✅ 修复点1：补上 VTK::CommonCore）
# =========================================================
file(GLOB SMESHDS_SRC src/SMESHDS/*.cpp)
add_library(SMESHDS SHARED ${SMESHDS_SRC})

target_include_directories(SMESHDS PRIVATE ${SMESH_INCLUDES})

target_link_libraries(SMESHDS PRIVATE
    SMDS
    ${SMESH_LIBS}
    VTK::CommonCore        # 🔥 必须加，解决 vtksys / vtkDebugLeaksManager
    VTK::CommonDataModel
)

# =========================================================
# DriverUNV（ ✅ 修复点2：必须放在 SMESHDS 之后）
# =========================================================
file(GLOB DriverUNV_SRC src/DriverUNV/*.cpp)
add_library(DriverUNV SHARED ${DriverUNV_SRC})

target_include_directories(DriverUNV PRIVATE ${SMESH_INCLUDES})

target_link_libraries(DriverUNV PRIVATE
    Driver
    SMDS
    SMESHDS
    ${SMESH_LIBS}
)

# =========================================================
# SMESH
# =========================================================
file(GLOB SMESH_SRC
    src/SMESH/*.cpp
    src/Controls/*.cpp
)

add_library(SMESH SHARED ${SMESH_SRC})

target_include_directories(SMESH PRIVATE ${SMESH_INCLUDES})

target_link_libraries(SMESH PRIVATE
    SMDS
    SMESHDS
    Driver
    DriverSTL
    DriverDAT
    DriverUNV

    ${SMESH_LIBS}

    VTK::CommonCore
    VTK::CommonSystem
    VTK::CommonDataModel
    VTK::FiltersCore
    VTK::RenderingCore
    VTK::RenderingOpenGL2
)

# =========================================================
# VTK 自动初始化（ ✅ 修复点3：覆盖 SMESHDS，否则依然链接失败）
# =========================================================
vtk_module_autoinit(
    TARGETS SMESH SMESHDS SMDS DriverSTL
    MODULES
        VTK::CommonCore
        VTK::CommonSystem
        VTK::CommonDataModel
        VTK::FiltersCore
        VTK::RenderingCore
        VTK::RenderingOpenGL2
)

# =========================================================
# MEFISTO2
# =========================================================
add_library(MEFISTO2 SHARED
    src/MEFISTO2/aptrte.cpp
    src/MEFISTO2/trte.c
)

target_include_directories(MEFISTO2 PRIVATE
    ${SMESH_INCLUDES}
    G:/vcpkg/installed/x64-windows/include
)

target_link_libraries(MEFISTO2 PRIVATE ${SMESH_LIBS})

# =========================================================
# StdMeshers
# =========================================================
file(GLOB StdMeshers_SRC src/StdMeshers/*.cpp)
add_library(StdMeshers SHARED ${StdMeshers_SRC})

target_include_directories(StdMeshers PRIVATE ${SMESH_INCLUDES})

target_link_libraries(StdMeshers PRIVATE
    SMESH
    MEFISTO2
    ${SMESH_LIBS}
)

# =========================================================
# NETGEN
# =========================================================
if(BUILD_FEM_NETGEN)
    file(GLOB NETGEN_SRC src/NETGENPlugin/*.cpp)
    add_library(NETGENPlugin SHARED ${NETGEN_SRC})

    target_link_libraries(NETGENPlugin PRIVATE
        SMESH
        SMDS
        StdMeshers
        ${SMESH_LIBS}
    )
endif()

# =========================================================
# install
# =========================================================
install(TARGETS
    SMDS Driver DriverSTL DriverDAT DriverUNV
    SMESHDS SMESH MEFISTO2 StdMeshers
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

if(BUILD_FEM_NETGEN)
    install(TARGETS NETGENPlugin
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
    )
endif()
