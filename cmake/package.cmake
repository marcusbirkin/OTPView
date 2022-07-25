cmake_minimum_required(VERSION 3.9)

# Locate QT Binary directory
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
get_target_property(QT_QMAKE_EXE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXE}" DIRECTORY)
file(REAL_PATH ${QT_BIN_DIR}/../lib/ QT_LIB_DIR)

# Variables common to all CPack Generators
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VENDOR "Marcus Birkin")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} ${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/marcusbirkin/OTPView")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_ICON "${${PROJECT_NAME}_ICON_PNG}")

# Set packing scripts
set(PROJECT_ICON ${CPACK_PACKAGE_ICON})
set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/package_prebuild.cmake")

# Select CPack Generator
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Use Qt Installer Framework
    include(package_ifw)
    set(USING_IFW TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Use Qt Installer Framework
    include(package_ifw)
    set(USING_IFW TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include(package_dmg)
else()
    message(SEND_ERROR "Unsupported platform for packer")
endif()

# Config Packing scripts
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/package_prebuild.cmake.in" 
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/package_prebuild.cmake"
    @ONLY
)

#--OTPView Main Application
cpack_add_component(
    ${PROJECT_NAME}
    DISPLAY_NAME ${PROJECT_NAME}
    DESCRIPTION "Main Application"
    REQUIRED
)
if(USING_IFW)
    cpack_ifw_configure_component(
        ${PROJECT_NAME}
        ESSENTIAL FORCED_INSTALLATION
        LICENSES "${PROJECT_NAME} License" "${PROJECT_SOURCE_DIR}/LICENSE"
        SORTING_PRIORITY 100
        VERSION ${PROJECT_VERSION}
        SCRIPT "${PROJECT_SOURCE_DIR}/installer/otpview_installscript.qs"
    )
endif()


#--OTPLib Support library
cpack_add_component(
    "OTPLib"
    DISPLAY_NAME "OTPLib"
    DESCRIPTION "Support library"
)
get_target_property(OTPLib_VERSION "OTPLib" VERSION)
if(USING_IFW)
    cpack_ifw_configure_component(
        "OTPLib"
        LICENSES "OTPLib License" "${PROJECT_SOURCE_DIR}/libs/OTPLib/LICENSE"
        VERSION ${OTPLib_VERSION}
    )
endif()

#--System Libraries (i.e. MSVC)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT "SYSTEM_RUNTIME")
    include(InstallRequiredSystemLibraries)
    cpack_add_component(
        "SYSTEM_RUNTIME"
        DISPLAY_NAME "Required System Libraries"
        HIDDEN
        REQUIRED
    )
endif()