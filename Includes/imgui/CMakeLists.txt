add_library(ImGui imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_widgets.cpp examples/imgui_impl_glfw.cpp examples/imgui_impl_opengl3.cpp ${PROJECT_SOURCE_DIR}/Shear3D/glad.c)
include_directories(${PROJECT_SOURCE_DIR}/Includes/imgui)
target_link_libraries(ImGui glfw)
