cmake_minimum_required(VERSION 3.18)
project(TowerDefense)

# Establecer estándar C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Especificar la ubicación de SFML
set(SFML_DIR "C:/SFML-2.6.1/lib/cmake/SFML")

# Encontrar SFML
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# Agregar ejecutable
add_executable(TowerDefense
    src/main.cpp
    src/map.cpp
    src/tower.cpp
    src/enemy.cpp
 "src/projectile.cpp")

# Vincular SFML al ejecutable
target_link_libraries(TowerDefense sfml-graphics sfml-window sfml-system)

# Copiar DLLs (para Windows)
if(WIN32)
    add_custom_command(TARGET TowerDefense POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/SFML-2.6.1/bin"
        $<TARGET_FILE_DIR:TowerDefense>)
endif()