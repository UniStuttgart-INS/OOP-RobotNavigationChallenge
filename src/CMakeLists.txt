message(STATUS "Building ${CMAKE_PROJECT_NAME}")

# Small projects better add their files manually. Big projects can search all files like below.

# Search all .cpp files in the current path
file(GLOB_RECURSE SRC_FILES "*.cpp")

# Add an executable with file name ${PROJECT_NAME_LOWERCASE} and a list of source files
# add_executable(${PROJECT_NAME_LOWERCASE} main.cpp)

# Add an executable with file name ${PROJECT_NAME_LOWERCASE} and Source files ${SRC_FILES}
add_executable(${PROJECT_NAME_LOWERCASE} ${SRC_FILES})

# ##################################################################################################

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME_LOWERCASE} PRIVATE project_options
                                                        project_warnings
                                                        application
                                                        imgui::imgui
                                                        implot::implot
                                                        fmt::fmt
                                                        spdlog::spdlog
                                                        Eigen3::Eigen)