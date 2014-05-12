# Makefile
########
#ARCH = `uname -s` . `uname -m`
#ARCH = Linux.i686
#SUNDIALS_DIR = /home/astro/sundials
########
# Some stuff that's the same for all programs and machines 
########

# The commands to call the C and C++ compilers
PKG_CONFIG_PATH = $(HPX_LOCATION)/lib/pkgconfig 
export PKG_CONFIG_PATH

CC = gcc
CXX = g++  -O3
F90 = gfortran

# Where to find the include files:
INCLUDE =  -I/home/zhangheng/boost_1_55_0 -I/home/zhangheng/dev/hpx_install/include

# library directory
LIB_DIR = -L/usr/local/lib -L/home/zhangheng/dev/hpx_install/lib/hpx

#LDLIBS = -lida.$(ARCH) -lshared.$(ARCH) -lnvecserial.$(ARCH) -lm
#LDLIBS = -lcvodes.$(ARCH) -lshared.$(ARCH) -lnvecserial.$(ARCH) -lm -lhdf5
LDLIBS =  -lm  -lboost_date_time -lboost_filesystem -lboost_program_options -lboost_regex -lboost_serialization -lboost_system -lboost_thread -lboost_atomic -lboost_chrono

CFLAGS = $(INCLUDE) $(LIB_DIR)

# Header files 
HDRS = 

########
# Finally, the commands that actually make stuff
# You shouldn't have to change anything here 
# These commands are of the form:
#   output_thing:  stuff it depends on
#   <TAB> command to make output_thing 
########

# re-link the program when the object files change
#all: ray fluid kerr kerr_fluid full_kerr
#all: kerr_geo kerr_fluid
all: bfsc

clean:
	rm -f bfsc *.o core*

bfsc:  bfsc.o validate.o
	$(CXX) $(CFLAGS) bfsc.o validate.o $(LDLIBS) -o bfsc 

# change a C object file whenever the corresponding source file
# or any of the header files changes
%.o : %.c $(HDRS) 
	$(CC) -c $(CFLAGS) $<
# What this means: 
# The %.o and %.c mean any .o file and the corresponding .c file
# All .c's are recompiled if you change one of the header files
# The $< symbol refers to the FIRST dependancy - so don't try to
# put $(HDRS) before %.c!

# change an object file whenever the corresponding source file
# or any of the header files changes
%.o : %.cpp $(HDRS) 
	$(CXX) -c $(CFLAGS) $<
%.o : %.F90 $(HDRS) 
	$(F90) -r8 -c $<
