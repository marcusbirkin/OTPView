# Download helper function
function(download_exe url target)
    # Download, if needed
    if(NOT EXISTS ${target})
        message("Downloading ${target}")
        file(DOWNLOAD 
            ${url}
            ${target}
            SHOW_PROGRESS
        )
    else()
        message("Skipping download of ${target}")
    endif()

    # Make executable
    file(
        CHMOD ${target}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    )
endfunction()

# Download linuxdeployqt
download_exe(
    "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    "linuxdeploy-x86_64.AppImage"
)
download_exe(
    "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
    "linuxdeploy-plugin-qt-x86_64.AppImage"
)
get_filename_component(
   LINUXDEPLOYQT "linuxdeploy-x86_64.AppImage"
   REALPATH
)

# Find all shared libs in CPack components
file(GLOB_RECURSE COMPONENTS_LIBS_FULLNAME "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/*.so*")
foreach(COMPONENTS_LIB ${COMPONENTS_LIBS_FULLNAME})
    get_filename_component(LIB_NAME ${COMPONENTS_LIB} NAME)
    list(APPEND COMPONENTS_LIBS_NAME ${LIB_NAME})

    get_filename_component(LIB_DIR ${COMPONENTS_LIB} DIRECTORY)
    list(APPEND COMPONENTS_LIBS_DIR ${LIB_DIR})

endforeach()
list(REMOVE_DUPLICATES COMPONENTS_LIBS_NAME)
list(REMOVE_DUPLICATES COMPONENTS_LIBS_DIR)

# Process each CPack component and deploy dependencies as an AppDir
foreach(COMPONENT ${CPACK_COMPONENTS_ALL})
    set(COMPONENT_DATA_DIR "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages/${COMPONENT}/data")
    
    file(GLOB COMPONENT_EXES "${COMPONENT_DATA_DIR}/bin/${COMPONENT}*")
    foreach(COMPONENT_EXE ${COMPONENT_EXES})
        message("Running Linuxdeplpyqt on ${COMPONENT_EXE}")

        # Pull component dependencies
        set(ENV{LD_LIBRARY_PATH} ${QT_LIB_DIR}:${COMPONENTS_LIBS_DIR})
        set(ENV{QMAKE} ${QT_QMAKE_EXE})
        set(LINUXDEPLOYQT_ARGS 
            "--plugin=qt"
            "--create-desktop-file"
            "--icon-file=${PROJECT_ICON}" "--icon-filename=${PROJECT_NAME}"
            "--appdir=${COMPONENT_DATA_DIR}"
            "--executable=${COMPONENT_EXE}"
        )
        # Exclude libs that are included in other components
        foreach(LIB_NAME ${COMPONENTS_LIBS_NAME})
            set(LINUXDEPLOYQT_ARGS 
                ${LINUXDEPLOYQT_ARGS}
                "--exclude-library=${LIB_NAME}"
            )
        endforeach()
        execute_process(
            COMMAND "${LINUXDEPLOYQT}" ${LINUXDEPLOYQT_ARGS}
        )

        # Rebase AppDir, moving everything in usr up on level
        message("Rebasing ${COMPONENT_DATA_DIR}/usr/ to ${COMPONENT_DATA_DIR}")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${COMPONENT_DATA_DIR}/usr/" "${COMPONENT_DATA_DIR}"
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${COMPONENT_DATA_DIR}/usr"
        )
        # Find and update any symbolic links referencing usr
        find_program(FIND NAMES find REQUIRED)
        find_program(AWK NAMES awk nawk mawk gawk REQUIRED)
        find_program(LINK NAMES ln REQUIRED)
        execute_process(
            COMMAND ${FIND} . -type l -lname "usr/*" -printf "%l %P\n"
            COMMAND ${AWK} "{ 
                gsub(\"usr/\", \"\")
                system(\"${LINK} -sf \"$1\" \"$2) 
                print \"Relinking \"$2\" -> \"$1
            }"
            WORKING_DIRECTORY ${COMPONENT_DATA_DIR}
        )
    endforeach()
endforeach()