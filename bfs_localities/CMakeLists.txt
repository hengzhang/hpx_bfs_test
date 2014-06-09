# Copyright (c) 2007-2009 Hartmut Kaiser
# Copyright (c) 2011      Bryce Lelbach
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
project(bfs_localities CXX)


set(CMAKE_MODULE_PATH ${HPX_ROOT}/share/cmake-2.8/Modules)
find_package(HPX REQUIRED)
include_directories(${HPX_INCLUDE_DIR})
link_directories(${HPX_LIBRARY_DIR})
#set(dependencies distributing_factory_component) 
#build your application using HPX
add_hpx_component(bfs_localities_component
  ESSENTIAL
  SOURCES bfs_localities_component.cpp
  HEADERS bfs_localities_component.hpp
  COMPONENT_DEPENDENCIES iostreams distributing_factory)

# build your application using HPX
add_hpx_executable(bfs_localities_client
  ESSENTIAL
  SOURCES bfs_localities_client.cpp
  COMPONENT_DEPENDENCIES bfs_localities_component distributing_factory
)



