cmake_minimum_required(VERSION 3.10)
project(SearchEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2")

set(BOOST_INCLUDEDIR $ENV{BOOST_HOME}/include)
set(BOOST_LIBRARYDIR $ENV{BOOST_HOME}/lib)
set(SEARCH_INCLUDEDIR include)
set(SEARCH_LIBRARYDIR lib)

set(Executable_DIRECTORY executable)
set(Libs_DIRECTORY lib)

find_package(Boost 1.58 COMPONENTS regex REQUIRED)

if(Boost_FOUND)
    include_directories(${BOOST_INCLUDEDIR})
    include_directories(${SEARCH_INCLUDEDIR})

    add_custom_target(executable_dir ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${Executable_DIRECTORY})
    add_custom_target(libs_dir ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${Libs_DIRECTORY})
    add_custom_target(index_dir ALL COMMAND ${CMAKE_COMMAND} -E make_directory index_files)


    add_library(tools SHARED ${SEARCH_INCLUDEDIR}/tools.cpp)
    set_target_properties(tools PROPERTIES COMPILE_FLAGS "-Wall -Werror -O2")

    add_library(index_maker SHARED
            ${SEARCH_INCLUDEDIR}/reader.cpp
            ${SEARCH_INCLUDEDIR}/input_handler.cpp
            ${SEARCH_INCLUDEDIR}/sorter.cpp
            ${SEARCH_INCLUDEDIR}/output_handler.cpp
            ${SEARCH_INCLUDEDIR}/writer.cpp
            ${SEARCH_INCLUDEDIR}/index_maker.cpp)
    set_target_properties(index_maker PROPERTIES COMPILE_FLAGS "-Wall -Werror -O2")
    target_link_libraries(index_maker tools)

    add_executable(tokenize tokenizer/tokenizer.cpp)
    add_executable(indexer index/make_index.cpp)
    add_executable(search boolean_search/make_search.cpp include/index.cpp include/expression_node.cpp)
    target_link_libraries(tokenize Boost::regex pthread tools)
    target_link_libraries(indexer tools index_maker)


    set_target_properties(search PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Executable_DIRECTORY}")
    set_target_properties(tokenize PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Executable_DIRECTORY}")
    set_target_properties(indexer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${Executable_DIRECTORY}")
endif()



