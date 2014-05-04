#!/bin/bash

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$HPX_LOCATION/lib/pkgconfig

# Compile the library
c++ -o libbfs_simple.so bfs_component.cpp `pkg-config --cflags --libs hpx_component` -DHPX_COMPONENT_NAME=bfs_simple

# Create the directory where we want to install the library
mkdir -p ~/my_hpx_libs
mv libbfs_simple.so ~/my_hpx_libs

# If we don't have a .hpx.ini yet, create one and tell it about
# our private hpx library directory
if [ ! -r ~/.hpx.ini ]
then
cat > ~/.hpx.ini <<EOF
[hpx]
ini_path = \$[hpx.ini_path]:${HOME}/my_hpx_libs
EOF
fi

# Create the ini file
cat > ~/my_hpx_libs/bfs_simple.ini <<EOF
[hpx.components.bfs_simple]
name = bfs_simple
path = ${HOME}/my_hpx_libs
EOF

# Compile the client
c++ -o bfs_simple_client bfs_simple_client.cpp `pkg-config --cflags --libs hpx_application` -liostreams -lbfs_simple -L ~/my_hpx_libs

# Prepare the environment so that we can run the command
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$HOME/my_hpx_libs"

# Run the client, first add our directory to the LD_LIBRARY_PATH
./bfs_simple_client

