set(project "SpaceInvaders")
project(${project})
cmake_minimum_required(VERSION 3.0)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

ADD_EXECUTABLE(${project} 
	source/main.cpp	
	source/headers/CubeMap.h
	source/headers/CubeMap.cpp
	source/headers/GameWorld.h
	source/headers/GameWorld.cpp
	source/headers/Shader.h
	source/headers/Shader.cpp
	source/headers/Model.h
	source/headers/Model.cpp
	source/headers/Mesh.h
	source/headers/Mesh.cpp
	source/headers/Vector.h
	source/headers/Vertex.h
	source/headers/Includes.h
)
TARGET_LINK_LIBRARIES(${project} ${CONAN_LIBS})

set (CMAKE_CXX_STANDARD 11)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${project})

FILE(COPY source/assets DESTINATION bin/)
