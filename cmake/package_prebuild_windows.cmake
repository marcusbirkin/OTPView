# Find WindeployQT
find_program(WINDEPLOYQT windeployqt HINTS "${QT_BIN_DIR}" REQUIRED)

# Find all shared libs in CPack components
file(GLOB_RECURSE COMPONENTS_LIBS_FULLNAME "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/*.dll*")
foreach(COMPONENTS_LIB ${COMPONENTS_LIBS_FULLNAME})
    get_filename_component(LIB_NAME ${COMPONENTS_LIB} NAME)
    list(APPEND COMPONENTS_LIBS_NAME ${LIB_NAME})

    get_filename_component(LIB_DIR ${COMPONENTS_LIB} DIRECTORY)
    list(APPEND COMPONENTS_LIBS_DIR ${LIB_DIR})

endforeach()
list(REMOVE_DUPLICATES COMPONENTS_LIBS_NAME)
list(REMOVE_DUPLICATES COMPONENTS_LIBS_DIR)

foreach(COMPONENT ${CPACK_COMPONENTS_ALL})
    set(COMPONENT_DATA_DIR "${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages/${COMPONENT}/data")
    
    file(GLOB COMPONENT_EXES "${COMPONENT_DATA_DIR}/bin/*.exe")
    foreach(COMPONENT_EXE ${COMPONENT_EXES})
        message("Running Windeplpyqt on ${COMPONENT_EXE}")

        # Pull component QT dependencies
        execute_process(
            COMMAND "${WINDEPLOYQT}" 
                --release
                --verbose 1
                --no-compiler-runtime
                --dir ${COMPONENT_DATA_DIR}/bin
                ${COMPONENT_EXE}
        )
    endforeach()
endforeach()