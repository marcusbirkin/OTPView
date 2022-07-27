cmake_minimum_required(VERSION 3.14)

# Icon
set(${PROJECT_NAME}_ICON_PNG "${PROJECT_SOURCE_DIR}/app/res/icon.png" CACHE FILEPATH "${PROJECT_NAME} Icon (PNG Format)")
set(${PROJECT_NAME}_ICON_ICNS "${PROJECT_SOURCE_DIR}/app/res/icon.icns" CACHE FILEPATH "${PROJECT_NAME} Icon (macOS ICNS Format)")
set(${PROJECT_NAME}_ICON_ICO "${PROJECT_SOURCE_DIR}/app/res/icon.ico" CACHE FILEPATH "${PROJECT_NAME} Icon (Windows ICO Format)") 

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(APP_RESOURCE "res/${PROJECT_NAME}.rc")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(APP_RESOURCE ${${PROJECT_NAME}_ICON_ICNS})
    get_filename_component(MACOSX_BUNDLE_ICON_FILE ${${PROJECT_NAME}_ICON_ICNS} NAME)
    set_source_files_properties(${${PROJECT_NAME}_ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

# Source files
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS "src/*.h")
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE PROJECT_UIS CONFIGURE_DEPENDS "src/*.ui")
file(GLOB_RECURSE PROJECT_QRCS CONFIGURE_DEPENDS "res/*.qrc")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    cmake_minimum_required(VERSION 3.19)
    qt_add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UIS}
        ${PROJECT_QRCS}
        ${APP_RESOURCE}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UIS}
        ${PROJECT_QRCS}
        ${APP_RESOURCE}
    )
endif()
target_include_directories(${PROJECT_NAME} PRIVATE "src")

# Installable items
install(TARGETS ${PROJECT_NAME} COMPONENT ${PROJECT_NAME})

# Create PDB files for Release
include(mscv_pdb)

# Qt
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network Charts)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network)

# OTPLib
target_link_libraries(${PROJECT_NAME} PUBLIC OTPLib)

# Version numbering
string(TIMESTAMP YEAR "%Y")
target_compile_definitions(${PROJECT_NAME} PRIVATE
    VER_FILEVERSION=${PROJECT_VERSION_MAJOR},${PROJECT_VERSION_MINOR},${PROJECT_VERSION_PATCH},${PROJECT_VERSION_TWEAK}
    VER_FILEVERSION_STR="${PROJECT_VERSION}"
    VER_PRODUCTVERSION=${PROJECT_VERSION}
    VER_PRODUCTVERSION_STR="${PROJECT_VERSION}"
    VER_COMPANYNAME_STR="Marcus Birkin"
    VER_FILEDESCRIPTION_STR="${PROJECT_NAME}"
    VER_INTERNALNAME_STR="${PROJECT_NAME}"
    VER_LEGALCOPYRIGHT_STR="2019 - ${YEAR} Marcus Birkin"
    VER_ORIGINALFILENAME_STR="${PROJECT_NAME}.exe"
    VER_PRODUCTNAME_STR="${PROJECT_NAME}"
)

# Binary properties
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     set_target_properties(${PROJECT_NAME} PROPERTIES
         WIN32_EXECUTABLE TRUE
     )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.marcusbirkin.otpview
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
    )
endif()

