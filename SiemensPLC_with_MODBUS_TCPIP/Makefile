# exectute in silent mode
.SILENT:

# output binary file name < editable >
out_binary:= _modbus_bin.elf
# took all cpp file from this location & use for build in g++
src_files:=$(wildcard src/*.cpp)

# initial build for project
# ****** entry point of this make tool **********
build:main.cpp
	g++ main.cpp -Iinc ${src_files} -g -o ${out_binary}

# clean binary if generated
clean: ${out_binary}
	rm -rf ${out_binary}

# run executable
run:build
	./${out_binary}

