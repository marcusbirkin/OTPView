# Find MacdeployQT
find_program(MACDEPLOYQT macdeployqt HINTS "${QT_BIN_DIR}" REQUIRED)

# Deploy dependencies in bundle
message("Copying libs from ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/lib into ${PROJECT_NAME}.app")
file(COPY
    ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/lib/
    DESTINATION ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/bin/${PROJECT_NAME}.app/Contents/Frameworks
    USE_SOURCE_PERMISSIONS
)

message("Running Macdeplpyqt on ${PROJECT_NAME}.app")
execute_process(
    COMMAND ${MACDEPLOYQT}
        ${PROJECT_NAME}.app
        -verbose=2
    WORKING_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/bin
)

# Copy app bundle to root of DMG
message("Cleaning up DMG layout")
file(COPY
    ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/bin/${PROJECT_NAME}.app
    DESTINATION ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/
    USE_SOURCE_PERMISSIONS
)

# Clean up old folders
execute_process(
    COMMAND ${CMAKE_COMMAND} -E rm -R bin/
    COMMAND ${CMAKE_COMMAND} -E rm -R lib/
    COMMAND ${CMAKE_COMMAND} -E rm -R include/
    WORKING_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/ALL_IN_ONE/
)
