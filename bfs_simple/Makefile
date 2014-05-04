


PKG_CONFIG_PATH = $(HPX_LOCATION)/lib/pkgconfig
export PKG_CONFIG_PATH

MY_HPX_LIBS = $(HOME)/my_hpx_libs

# Prepare the environment so that we can run the command
LD_LIBRARY_PATH = $(MY_HPX_LIBS)
export LD_LIBRARY_PATH

all : run

# Compile the library
libbfs_simple.so : bfs_component.cpp
	c++ -o libbfs_simple.so bfs_component.cpp `pkg-config --cflags --libs hpx_component` -DHPX_COMPONENT_NAME=bfs_simple

# Move the library to the library directory
$(MY_HPX_LIBS)/libbfs_simple.so: libbfs_simple.so
	mkdir -p $(MY_HPX_LIBS)
	cp libbfs_simple.so $(MY_HPX_LIBS)

# Create the hpx.ini file
$(HOME)/.hpx.ini:
	echo "[hpx]" > $(HOME)/.hpx.ini
	echo "ini_path = \$$[hpx.ini_default_path]:$(MY_HPX_LIBS)" >> $(HOME)/.hpx.ini

# Create the ini file
$(MY_HPX_LIBS)/bfs_simple.ini:
	echo "$(MY_HPX_LIBS)/bfs_simple.ini" > $(MY_HPX_LIBS)/bfs_simple.ini
	echo "[hpx.components.bfs_simple]" >> $(MY_HPX_LIBS)/bfs_simple.ini
	echo "name = bfs_simple" >> $(MY_HPX_LIBS)/bfs_simple.ini
	echo "path = $(MY_HPX_LIBS)" >> $(MY_HPX_LIBS)/bfs_simple.ini

# Compile the client
bfs_simple_client : bfs_simple_client.cpp $(MY_HPX_LIBS)/libbfs_simple.so
	c++ -o bfs_simple_client bfs_simple_client.cpp `pkg-config --cflags --libs hpx_application` -liostreams -lbfs_simple -L $(MY_HPX_LIBS)

# Run the client, first add our directory to the LD_LIBRARY_PATH
run: bfs_simple_client $(HOME)/.hpx.ini $(MY_HPX_LIBS)/bfs_simple.ini
	./bfs_simple_client

clean:
	rm -f bfs_simple_client libbfs_simple.so
