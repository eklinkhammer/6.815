# C/C++ compilation involves two stages:
# - compiling: convert each source (*.cpp) file indepdendently into binary object (.o) files
# - linking: assemble all the object files into a single executable
#
# Since any C++ project contains multiple source files, the above
# process involved firing multiple commands. Makefiles help us
# write all the compilation stages in a single file and run them
# as one big script.
#
# See http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# and many many other tutorials for makefiles


# some variables
BUILD_DIR  :=_build
EXECUTABLE := a5
OUTPUT := Output

# the C++ compiler/linker to be used. define here so that we can change
# it easily if needed
CXX := g++ -Wall

# ------------------------------------------------------------------------------

# 'make' or 'make all' runs the default target 'all' which requires that
# the EXECUTABLE must be available; if the EXECUTABLE is already available
# then nothing happens. see the rules for EXECUTABLE

all: $(EXECUTABLE)

# ------------------------------------------------------------------------------

# 'make run' runs the target 'run' which calls for running the executable
# if the executable is not available, then the rules for creating it are run
# this is a superset of the target 'all' because it creates the executable
# and also runs it

run: $(EXECUTABLE)
	./$(EXECUTABLE)

# ------------------------------------------------------------------------------

# 'make clean' runs the target 'clean' which in turn removes the
# intermediate .o files and the executable

clean:
	rm -rf $(BUILD_DIR) $(EXECUTABLE) $(OUTPUT)

# ------------------------------------------------------------------------------

# rule for creating the executable: this "links" the .o files using the g++ linker.
# If .o files are not available, then the rules for creating .o files are run.

$(EXECUTABLE): $(BUILD_DIR)/a5_main.o $(BUILD_DIR)/hdr.o $(BUILD_DIR)/basicImageManipulation.o $(BUILD_DIR)/filtering.o $(BUILD_DIR)/Image.o $(BUILD_DIR)/lodepng.o
	$(CXX) $(BUILD_DIR)/a5_main.o $(BUILD_DIR)/hdr.o $(BUILD_DIR)/basicImageManipulation.o $(BUILD_DIR)/filtering.o $(BUILD_DIR)/Image.o $(BUILD_DIR)/lodepng.o -o $(EXECUTABLE)
	mkdir -p $(OUTPUT)

# ------------------------------------------------------------------------------

# rules for creating the .o files:  compile each of the .cpp files and create a
# corresponding .o file. Each .o depends upon the corresponding .cpp and other .h files



$(BUILD_DIR)/hdr.o: hdr.cpp hdr.h Image.h basicImageManipulation.h ImageException.h lodepng.h messageassert.h
	mkdir -p $(BUILD_DIR)
	$(CXX) -c hdr.cpp -o $(BUILD_DIR)/hdr.o

$(BUILD_DIR)/filtering.o: filtering.cpp filtering.h Image.h basicImageManipulation.h ImageException.h lodepng.h messageassert.h
	mkdir -p $(BUILD_DIR)
	$(CXX) -c filtering.cpp -o $(BUILD_DIR)/filtering.o

$(BUILD_DIR)/a5_main.o: a5_main.cpp filtering.h hdr.h basicImageManipulation.h Image.h ImageException.h  lodepng.h messageassert.h
	mkdir -p $(BUILD_DIR)
	$(CXX) -c a5_main.cpp -o $(BUILD_DIR)/a5_main.o

$(BUILD_DIR)/basicImageManipulation.o: basicImageManipulation.cpp basicImageManipulation.h Image.h ImageException.h lodepng.h messageassert.h
	mkdir -p $(BUILD_DIR)
	$(CXX) -c basicImageManipulation.cpp -o $(BUILD_DIR)/basicImageManipulation.o

$(BUILD_DIR)/Image.o: Image.cpp Image.h ImageException.h lodepng.h messageassert.h
	mkdir -p $(BUILD_DIR)
	$(CXX) -c Image.cpp -o $(BUILD_DIR)/Image.o

$(BUILD_DIR)/lodepng.o: lodepng.cpp ImageException.h lodepng.h messageassert.h
	mkdir -p $(BUILD_DIR)
	$(CXX) -c lodepng.cpp -o $(BUILD_DIR)/lodepng.o
