cmake_minimum_required(VERSION 3.16)
project(TwitchAppProject VERSION 1.0 LANGUAGES C CXX)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.86.0 REQUIRED) 
find_package(SFML 3 COMPONENTS Window Graphics System REQUIRED)

if(Boost_FOUND AND SFML_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(TwitchApp src/main.cpp) 
    target_link_libraries(TwitchApp PRIVATE ${Boost_LIBRARIES})
    target_link_libraries(TwitchApp PRIVATE SFML::Graphics SFML::Window SFML::System)
endif()

target_compile_features(TwitchApp PRIVATE cxx_std_20)


add_custom_command(
    TARGET TwitchApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/src/FiraCode-Regular.ttf
        $<TARGET_FILE_DIR:TwitchApp>/FiraCode-Regular.ttf
)

if(WIN32)
    add_custom_command(TARGET TwitchApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:TwitchApp>
            $<TARGET_FILE_DIR:TwitchApp>
        COMMAND_EXPAND_LISTS
    )
endif()