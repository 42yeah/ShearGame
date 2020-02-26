# Why can't cmake integrate this already?
macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    @ONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

# Include the includes
include_directories(${PROJECT_SOURCE_DIR}/Includes)
add_executable(Shear3D main.cpp tests.cpp Program.cpp Pass.cpp Game.cpp Camera.cpp Model.cpp Texture.cpp Object.cpp Monster.cpp Item.cpp)

target_link_libraries(Shear3D ImGui glfw)
file(COPY ${PROJECT_SOURCE_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
