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
    FiltersVerdict
)

# =========================
# OpenCASCADE
# =========================
set(OCC_ROOT "G:/OCCT/INSTALL")
set(OCC_LIBS
    # 核心基础
    ${OCC_ROOT}/lib/TKernel.lib
    ${OCC_ROOT}/lib/TKMath.lib

    # 几何 & 拓扑
    ${OCC_ROOT}/lib/TKG3d.lib
    ${OCC_ROOT}/lib/TKG2d.lib
    ${OCC_ROOT}/lib/TKGeomBase.lib
    ${OCC_ROOT}/lib/TKGeomAlgo.lib
    ${OCC_ROOT}/lib/TKBRep.lib
    ${OCC_ROOT}/lib/TKTopAlgo.lib

    # 布尔、修复、缝合（BOPTools 必用）
    ${OCC_ROOT}/lib/TKBool.lib
    ${OCC_ROOT}/lib/TKBO.lib
    ${OCC_ROOT}/lib/TKShHealing.lib
    ${OCC_ROOT}/lib/TKFeat.lib

    # 网格
    ${OCC_ROOT}/lib/TKMesh.lib
    ${OCC_ROOT}/lib/TKMeshVS.lib

    # 图形 & 显示
    ${OCC_ROOT}/lib/TKService.lib
    ${OCC_ROOT}/lib/TKOpenGl.lib

    # ===================== XML / LDOM / 文档解析（本次报错核心） =====================
    ${OCC_ROOT}/lib/TKCDF.lib
    ${OCC_ROOT}/lib/TKPrim.lib
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
    G:/vcpkg/installed/x64-windows/include/vtk-9.3
    G:/vcpkg/installed/x64-windows/include
    G:/MEDFileOuput/include/
    F:/libMeshb/install/include
)

set(VTK_ROOT "G:/vcpkg/installed/x64-windows")
set(VTK_LIBS
    ${VTK_ROOT}/lib/vtkCommonCore-9.3d.lib
    ${VTK_ROOT}/lib/vtkCommonDataModel-9.3d.lib
    ${VTK_ROOT}/lib/vtkFiltersCore-9.3d.lib
    ${VTK_ROOT}/lib/vtkIOCore-9.3d.lib
    ${VTK_ROOT}/lib/vtkCommonExecutionModel-9.3d.lib
    ${VTK_ROOT}/lib/vtksys-9.3d.lib
    # 核心补充：解决 vtkMeshQuality 链接错误
    ${VTK_ROOT}/lib/vtkFiltersGeneric-9.3d.lib
    ${VTK_ROOT}/lib/vtkFiltersVerdict-9.3d.lib

)

# =========================
# Boost（vcpkg 自动查找）
# =========================
find_package(Boost CONFIG REQUIRED)

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

set(MED_LIBS
    G:/MEDFileOuput/lib/medC.lib
    G:/MEDFileOuput/lib/medimport.lib
)

set(SMESH_LIBS
    Rpcrt4.lib
    F:/libMeshb/install/lib/Meshb.8.lib
    ${MED_LIBS}
    ${OCC_LIBS}
    ${VTK_LIBS}
)

target_include_directories(SMDS PRIVATE ${SMESH_INCLUDES})
target_link_libraries(SMDS PRIVATE
    ${SMESH_LIBS}
)

# =========================================================
# Driver
# =========================================================
file(GLOB Driver_SRC src/Driver/*.cpp)
add_library(Driver SHARED ${Driver_SRC})

target_include_directories(Driver PRIVATE ${SMESH_INCLUDES})

find_package(Boost REQUIRED COMPONENTS
    thread
    system
    chrono
    date_time
)

target_link_libraries(Driver PUBLIC
    Boost::thread
    Boost::system
    Boost::chrono
    Boost::date_time
)

# =========================================================
# DriverSTL
# =========================================================
file(GLOB DriverSTL_SRC src/DriverSTL/*.cpp)
add_library(DriverSTL SHARED ${DriverSTL_SRC})
target_include_directories(DriverSTL PRIVATE ${SMESH_INCLUDES})
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
    Boost::thread
    Boost::system
    Boost::chrono
    Boost::date_time
)

# =========================================================
# SMESHDS
# =========================================================
file(GLOB SMESHDS_SRC src/SMESHDS/*.cpp)
add_library(SMESHDS SHARED ${SMESHDS_SRC})
target_include_directories(SMESHDS PRIVATE ${SMESH_INCLUDES})
target_link_libraries(SMESHDS PUBLIC
    SMDS
    ${SMESH_LIBS}
    Boost::thread
    Boost::system
    Boost::chrono
    Boost::date_time
)

# =========================================================
# DriverUNV
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
target_link_libraries(SMESH PUBLIC
    SMDS
    SMESHDS
    Driver
    DriverSTL
    DriverDAT
    DriverUNV
    ${SMESH_LIBS}
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
)
target_link_libraries(MEFISTO2)

# =========================================================
# StdMeshers
# =========================================================
file(GLOB StdMeshers_SRC src/StdMeshers/*.cpp)
add_library(StdMeshers SHARED ${StdMeshers_SRC})
target_include_directories(StdMeshers PRIVATE ${SMESH_INCLUDES})
target_link_libraries(StdMeshers PRIVATE
    SMESH
    MEFISTO2
)

# =========================================================
# install
# =========================================================
install(TARGETS
    SMDS Driver DriverSTL DriverDAT DriverUNV
    SMESHDS SMESH MEFISTO2 StdMeshers
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
