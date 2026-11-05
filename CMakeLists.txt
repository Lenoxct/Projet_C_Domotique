cmake_minimum_required(VERSION 3.16)
project(domotique LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include(FetchContent)

FetchContent_Declare(
    webview
    GIT_REPOSITORY https://github.com/webview/webview
    GIT_TAG 0.12.0)
FetchContent_MakeAvailable(webview)

add_executable(domotique WIN32)
target_sources(domotique PRIVATE main.c database.c sqlite3.c)
target_include_directories(domotique PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(domotique PRIVATE SQLITE_THREADSAFE=1)
target_link_libraries(domotique PRIVATE webview::core_static ws2_32)

add_custom_command(TARGET domotique POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
        "$<TARGET_FILE_DIR:domotique>/index.html"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/style.css"
        "$<TARGET_FILE_DIR:domotique>/style.css"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Database.sql"
        "$<TARGET_FILE_DIR:domotique>/Database.sql"
)