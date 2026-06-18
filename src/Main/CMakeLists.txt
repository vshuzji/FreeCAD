# SPDX-License-Identifier: LGPL-2.1-or-later

configure_file(freecad.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/freecad.rc)
configure_file(freecadCmd.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/freecadCmd.rc)
file(COPY icon.ico DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/3rdParty/PyCXX)
# =========================
# Qt6 + fmt（保留）
# =========================
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Xml)
find_package(fmt CONFIG REQUIRED)
find_package(Python REQUIRED COMPONENTS Interpreter Development)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

######################## FreeCADMain ########################
if(BUILD_GUI)

    SET(FreeCAD_SRCS
        ${CMAKE_CURRENT_BINARY_DIR}/freecad.rc
        icon.ico
        MainGui.cpp
    )

add_executable(FreeCADMain WIN32 ${FreeCAD_SRCS})

target_link_libraries(FreeCADMain
    PRIVATE
    FreeCADGui
    FreeCADApp
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

if(BUILD_TEST)
    add_dependencies(FreeCADMain TestSources)
    add_dependencies(FreeCADMain TestGuiSources)
endif()

if(FREECAD_WARN_ERROR)
    target_compile_warn_error(FreeCADMain)
endif()

SET_BIN_DIR(FreeCADMain FreeCAD)

if(WIN32)
    INSTALL(TARGETS FreeCADMain
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
elseif(APPLE AND NOT BUILD_WITH_CONDA)
    INSTALL(TARGETS FreeCADMain
        RUNTIME DESTINATION MacOS
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    INSTALL(TARGETS FreeCADMain
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

endif(BUILD_GUI)

######################## FreeCADMainCmd ########################

SET(FreeCADMainCmd_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/freecadCmd.rc
    icon.ico
    MainCmd.cpp
)

add_executable(FreeCADMainCmd ${FreeCADMainCmd_SRCS})

if(BUILD_TEST)
    add_dependencies(FreeCADMainCmd TestSources)
endif()

target_link_libraries(FreeCADMainCmd
    PRIVATE
    FreeCADApp
    Qt6::Core
    fmt::fmt
    Python::Python
)

if(FREECAD_WARN_ERROR)
    target_compile_warn_error(FreeCADMainCmd)
endif()

SET_BIN_DIR(FreeCADMainCmd FreeCADCmd)

if(WIN32)
    INSTALL(TARGETS FreeCADMainCmd
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
elseif(APPLE AND NOT BUILD_WITH_CONDA)
    INSTALL(TARGETS FreeCADMainCmd
        RUNTIME DESTINATION MacOS
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    INSTALL(TARGETS FreeCADMainCmd
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

######################## FreeCADMainPy ########################

SET(FreeCADMainPy_SRCS
    MainPy.cpp
)

add_library(FreeCADMainPy SHARED ${FreeCADMainPy_SRCS})

target_link_libraries(FreeCADMainPy
    PRIVATE
    FreeCADApp
    Qt6::Core
    fmt::fmt
    Python::Python
)

if(FREECAD_WARN_ERROR)
    target_compile_warn_error(FreeCADMainPy)
endif()

SET_BIN_DIR(FreeCADMainPy FreeCAD)
SET_PYTHON_PREFIX_SUFFIX(FreeCADMainPy)

if(WIN32)
    INSTALL(TARGETS FreeCADMainPy
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    INSTALL(TARGETS FreeCADMainPy
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

######################## FreeCADGuiPy ########################
if(BUILD_GUI)

    set(FreeCADGuiPy_SRCS
        FreeCADGuiPy.cpp
    )

add_library(FreeCADGuiPy SHARED ${FreeCADGuiPy_SRCS})

target_link_libraries(FreeCADGuiPy
    PRIVATE
    FreeCADGui
    FreeCADApp
)

set_target_properties(FreeCADGuiPy PROPERTIES
    DEBUG_POSTFIX "_d"
    OUTPUT_NAME "FreeCADGuiPy"
    PDB_NAME_DEBUG "FreeCADGuiPy_d"
    PDB_NAME_RELEASE "FreeCADGuiPy"
)

SET_BIN_DIR(FreeCADGuiPy FreeCADGui)
SET_PYTHON_PREFIX_SUFFIX(FreeCADGuiPy)

if(WIN32)

    set_target_properties(FreeCADGuiPy PROPERTIES
        ARCHIVE_OUTPUT_NAME "FreeCADGuiPy"
        LIBRARY_OUTPUT_NAME "FreeCADGuiPy"
        RUNTIME_OUTPUT_NAME "FreeCADGuiPy"

        ARCHIVE_OUTPUT_NAME_DEBUG "FreeCADGuiPy_d"
        LIBRARY_OUTPUT_NAME_DEBUG "FreeCADGuiPy_d"
        RUNTIME_OUTPUT_NAME_DEBUG "FreeCADGuiPy_d"
    )

endif()

if(WIN32)
    install(TARGETS FreeCADGuiPy
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else()
    install(TARGETS FreeCADGuiPy
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

endif(BUILD_GUI)
