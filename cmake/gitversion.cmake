cmake_minimum_required(VERSION 3.0)

find_package(Git REQUIRED)

message("Extracting version from git tag for ${CMAKE_CURRENT_SOURCE_DIR}")

# Extract git tag
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --always --tags
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE GITVERSION_TAG)
string(STRIP ${GITVERSION_TAG} GITVERSION_TAG)
message("Tag: ${GITVERSION_TAG}")

# Convert tag into version list
string(REGEX REPLACE "^[v]" "" GITVERSION_TAG ${GITVERSION_TAG}) # Prepending v, if any
string(REGEX REPLACE "[.]|[-]" ";" GITVERSION_TAG_LIST ${GITVERSION_TAG}) # Split into list based on . and -

# And convert list into PROJECT_VERSION_x variables
list(LENGTH GITVERSION_TAG_LIST GITVERSION_TAG_LIST_LEN)
list(GET GITVERSION_TAG_LIST 0 PROJECT_VERSION_MAJOR)
list(GET GITVERSION_TAG_LIST 1 PROJECT_VERSION_MINOR)
list(GET GITVERSION_TAG_LIST 2 PROJECT_VERSION_PATCH)
message(fatel_error ${GITVERSION_TAG_LIST_LEN})
if (${GITVERSION_TAG_LIST_LEN} LESS 4)
    set(PROJECT_VERSION_TWEAK 0)
else()
    list(GET GITVERSION_TAG_LIST 3 PROJECT_VERSION_TWEAK)
endif()
message("Interpreted Version: ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK}")
