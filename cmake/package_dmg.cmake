cmake_minimum_required(VERSION 3.9)

# DMX Drag and Drop installer
set(CPACK_GENERATOR "DragNDrop")

# Volume Name
set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME} ${PROJECT_VERSION}")

# Set volume read only
set(CPACK_DMG_FORMAT UDRO)

# Most people don't know macOS is Darwin
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-macOS")
endif()

include(CPack)
