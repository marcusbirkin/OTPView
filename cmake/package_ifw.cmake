cmake_minimum_required(VERSION 3.9)

# Qt Installer Framework
set(CPACK_GENERATOR "IFW")
set(CPACK_IFW_VERBOSE ON)

# Product URL
set(CPACK_IFW_PRODUCT_URL ${CPACK_PACKAGE_HOMEPAGE_URL})

# Icon
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_IFW_PACKAGE_ICON ${PROJECT_ICON_PNG})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_IFW_PACKAGE_ICON  ${PROJECT_ICON_ICO})
endif()
set(CPACK_IFW_PACKAGE_WINDOW_ICON ${PROJECT_ICON_PNG})

# Default install folder
set(CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDir@/${PROJECT_NAME}")

# Windows Start menu group
set(CPACK_IFW_PACKAGE_START_MENU_DIRECTORY ${PROJECT_NAME})

# Maintenance Tool filename
set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "Uninstaller")

# Installer control script
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT "${PROJECT_SOURCE_DIR}/installer/controlscript.qs")

# Installer Wizard options
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Modern")
set(CPACK_IFW_PACKAGE_WIZARD_SHOW_PAGE_LIST OFF)

include(CPack)
include(CPackIFW)
