project(arbre)

find_package(Threads)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
add_compile_options(-Wall -Wpedantic)


set(SRCS
    noeud.cc
    relation.cc
    arbre.cc
    montecarlo.cc
    )

set(HEADERS
    noeud.hh
    relation.hh
    arbre.hh
    montecarlo.hh
    )

add_library(arbre ${SRCS} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
