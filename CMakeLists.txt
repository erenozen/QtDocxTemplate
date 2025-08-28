cmake_minimum_required(VERSION 3.16)
project(QtDocxTemplate VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)
find_package(libzip QUIET)
if(libzip_FOUND)
    message(STATUS "Using system libzip")
    set(QTDOCTEMPLATE_USE_LIBZIP TRUE)
else()
    include(FetchContent)
    # Minizip-ng fallback
    if(NOT DEFINED MINIZIP_NG_TAG)
        set(MINIZIP_NG_TAG 4.0.4) # reasonable stable tag
    endif()
    set(MZ_OPENSSL OFF CACHE INTERNAL "")
    FetchContent_Declare(minizip_ng
        GIT_REPOSITORY https://github.com/nmoinvaz/minizip.git
        GIT_TAG ${MINIZIP_NG_TAG}
    )
    set(MZ_BUILD_TESTS OFF CACHE INTERNAL "")
    set(MZ_BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(MZ_BUILD_UNIT_TESTS OFF CACHE INTERNAL "")
    set(MZ_ZLIB ON CACHE INTERNAL "")
    FetchContent_MakeAvailable(minizip_ng)
endif()

add_library(QtDocxTemplate
    src/Docx.cpp
    src/Variables.cpp
    src/Variable.cpp
    src/TextVariable.cpp
    src/ImageVariable.cpp
    src/BulletListVariable.cpp
    src/TableVariable.cpp
    src/opc/Package.cpp
    src/xml/XmlPart.cpp
    src/engine/RunModel.cpp
    src/engine/Replacers.cpp
    src/util/Emu.hpp
)

add_library(QtDocxTemplate::core ALIAS QtDocxTemplate)

target_include_directories(QtDocxTemplate
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(QtDocxTemplate PUBLIC cxx_std_17)

target_link_libraries(QtDocxTemplate
    PUBLIC
        Qt6::Core
        Qt6::Gui
    PRIVATE
        $<$<BOOL:${QTDOCTEMPLATE_USE_LIBZIP}>:libzip::zip>
    $<$<NOT:$<BOOL:${QTDOCTEMPLATE_USE_LIBZIP}>>:MINIZIP::minizip>
)

target_compile_definitions(QtDocxTemplate PRIVATE QTDOCTEMPLATE_BUILD_LIB)
if(QTDOCTEMPLATE_USE_LIBZIP)
    target_compile_definitions(QtDocxTemplate PRIVATE QTDOCTEMPLATE_USE_LIBZIP=1)
else()
    target_compile_definitions(QtDocxTemplate PRIVATE QTDOCTEMPLATE_USE_MINIZIP=1)
endif()

install(TARGETS QtDocxTemplate
    EXPORT QtDocxTemplateTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/QtDocxTemplateConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QtDocxTemplateConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtDocxTemplate
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/QtDocxTemplateConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtDocxTemplate
)

install(EXPORT QtDocxTemplateTargets
    NAMESPACE QtDocxTemplate::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtDocxTemplate
)

option(QTDOCTXTEMPLATE_BUILD_EXAMPLES "Build examples" ON)
option(QTDOCTXTEMPLATE_BUILD_TESTS "Build tests" ON)

if(QTDOCTXTEMPLATE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(QTDOCTXTEMPLATE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "Configured QtDocxTemplate ${PROJECT_VERSION}")
