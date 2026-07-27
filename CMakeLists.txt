cmake_minimum_required(VERSION 3.16)
project(QuanLyDiem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ------------------------------------------------------------
# 1. Tự động tải GLFW (thư viện tạo cửa sổ) từ GitHub
# ------------------------------------------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ------------------------------------------------------------
# 2. Tự động tải Dear ImGui từ GitHub
# ------------------------------------------------------------
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.9
)
FetchContent_MakeAvailable(imgui)

# ------------------------------------------------------------
# 3. Tự động tải SQLite amalgamation (1 file .c + .h duy nhất)
# ------------------------------------------------------------
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip
)
FetchContent_MakeAvailable(sqlite3)

# ------------------------------------------------------------
# 4. Gộp source code ImGui (chỉ cần các file lõi + backend GLFW/OpenGL)
# ------------------------------------------------------------
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# ------------------------------------------------------------
# 5. Định nghĩa file thực thi chính, gộp cả source project lẫn thư viện
# ------------------------------------------------------------
add_executable(QuanLyDiem
    src/main.cpp
    src/SinhVien.cpp
    src/LinkedList.cpp
    src/Bst.cpp
    src/Database.cpp
    src/GUI.cpp
    ${IMGUI_SOURCES}
    ${sqlite3_SOURCE_DIR}/sqlite3.c
)

target_include_directories(QuanLyDiem PRIVATE
    include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${sqlite3_SOURCE_DIR}
)

target_link_libraries(QuanLyDiem PRIVATE glfw)

# Trên Windows cần link thêm OpenGL, trên Linux cần thêm dl + pthread
if (WIN32)
    target_link_libraries(QuanLyDiem PRIVATE opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(QuanLyDiem PRIVATE OpenGL::GL ${CMAKE_DL_LIBS} pthread)
endif()

# ------------------------------------------------------------
# 6. Cấu hình DEPLOY: gộp tĩnh (static) runtime của MinGW vào file .exe
# để máy khác chạy được mà KHÔNG cần cài MinGW hay bất kỳ DLL nào thêm.
# Chỉ áp dụng khi build ở chế độ Release (dùng để phát hành/nộp bài).
# ------------------------------------------------------------
if (MINGW)
    target_link_options(QuanLyDiem PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
    )
endif()