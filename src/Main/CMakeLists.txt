# SPDX-License-Identifier: LGPL-2.1-or-later

configure_file(freecad.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/freecad.rc)
configure_file(freecadCmd.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/freecadCmd.rc)
file(COPY icon.ico DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

######################## FreeCADMain ########################
if(BUILD_GUI)

    SET(FreeCAD_SRCS
        ${CMAKE_CURRENT_BINARY_DIR}/freecad.rc
        icon.ico
        MainGui.cpp
    )

    SET(FreeCAD_LIBS
        FreeCADGui
    )

    if(NOT BUILD_DYNAMIC_LINK_PYTHON)
        # executables have to be linked against python libraries,
        # because extension modules are not.
        list(APPEND FreeCAD_LIBS
            ${Python3_LIBRARIES}
        )
    endif(NOT BUILD_DYNAMIC_LINK_PYTHON)

    add_executable(FreeCADMain WIN32 ${FreeCAD_SRCS})
    target_link_libraries(FreeCADMain ${FreeCAD_LIBS})
    if(BUILD_TEST)
        add_dependencies(FreeCADMain TestSources)
        add_dependencies(FreeCADMain TestGuiSources)
    endif()
    if (FREECAD_WARN_ERROR)
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

SET(FreeCADMainCmd_LIBS
    FreeCADApp
    ${QtCore_LIBRARIES}
    ${QtXml_LIBRARIES}
)

if(NOT BUILD_DYNAMIC_LINK_PYTHON)
    # executables have to be linked against python libraries,
    # because extension modules are not.
    list(APPEND FreeCADMainCmd_LIBS
        ${Python3_LIBRARIES}
    )
endif(NOT BUILD_DYNAMIC_LINK_PYTHON)

target_link_libraries(FreeCADMainCmd
    ${FreeCADMainCmd_LIBS}
)
if (FREECAD_WARN_ERROR)
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

target_link_libraries(FreeCADMainPy FreeCADApp)
if (FREECAD_WARN_ERROR)
    target_compile_warn_error(FreeCADMainPy)
endif()

SET_BIN_DIR(FreeCADMainPy FreeCAD)
SET_PYTHON_PREFIX_SUFFIX(FreeCADMainPy)

if(WIN32)
    INSTALL(TARGETS FreeCADMainPy
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
else(WIN32)
    INSTALL(TARGETS FreeCADMainPy
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif(WIN32)

######################## FreeCADGuiPy ########################
if(BUILD_GUI)
    set(FreeCADGuiPy_SRCS
            FreeCADGuiPy.cpp
        )

        add_library(FreeCADGuiPy SHARED ${FreeCADGuiPy_SRCS})

        target_link_libraries(FreeCADGuiPy
            PRIVATE
            FreeCADGui
        )

        # =========================
        # 关键修复：避免 Ninja / MSVC 输出冲突
        # =========================
        set_target_properties(FreeCADGuiPy PROPERTIES

            # 防止 Debug/Release 共享同名 lib
            DEBUG_POSTFIX "_d"
            RELEASE_POSTFIX ""

            # 明确输出名字（避免和 FreeCADGui 冲突）
            OUTPUT_NAME "FreeCADGuiPy"

            # PDB 防冲突（MSVC / Ninja Debug）
            PDB_NAME_DEBUG "FreeCADGuiPy_d"
            PDB_NAME_RELEASE "FreeCADGuiPy"
        )

        # =========================
        # Python module 属性（非常重要）
        # =========================
        SET_BIN_DIR(FreeCADGuiPy FreeCADGui)
        SET_PYTHON_PREFIX_SUFFIX(FreeCADGuiPy)

        # =========================
        # Windows 特殊处理（避免 .lib 冲突）
        # =========================
        if(WIN32)

            # Python extension DLL 不要和 .lib 名字混乱
            set_target_properties(FreeCADGuiPy PROPERTIES
                ARCHIVE_OUTPUT_NAME "FreeCADGuiPy"
                LIBRARY_OUTPUT_NAME "FreeCADGuiPy"
                RUNTIME_OUTPUT_NAME "FreeCADGuiPy"
            )

            # Debug 明确后缀（防 Ninja 生成冲突）
            set_target_properties(FreeCADGuiPy PROPERTIES
                ARCHIVE_OUTPUT_NAME_DEBUG "FreeCADGuiPy_d"
                LIBRARY_OUTPUT_NAME_DEBUG "FreeCADGuiPy_d"
                RUNTIME_OUTPUT_NAME_DEBUG "FreeCADGuiPy_d"
            )

        endif()

        # =========================
        # install
        # =========================
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
