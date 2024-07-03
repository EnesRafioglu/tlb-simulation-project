#* Find source and header files in the current directory and its subdirectories
SOURCES_C    := $(shell find * -name '*.c')
SOURCES_CPP  := $(shell find * -name '*.cpp')

HEADERS_C    := $(shell find * -name '*.h')
HEADERS_CPP  := $(shell find * -name '*.hpp')

OBJECTS_C    := $(addprefix build/,$(SOURCES_C:.c=.o))
OBJECTS_CPP  := $(addprefix build/,$(SOURCES_CPP:.cpp=.o))

SRC_DIRS     := $(shell find src -type d)

SYSTEMC_PATH := ../systemc

BUILD_DIR    := build
OUTPUT       := tla


#* Define the compiler and flags
COMPILER_FLAGS_C       := -std=gnu17 -Wall -Wextra -DLANG_C
COMPILER_FLAGS_CPP     := -std=gnu++14 -Wall -Wextra -I$(SYSTEMC_PATH)/include -DLANG_CPP

LINKER_FLAGS           := -L$(SYSTEMC_PATH)/lib -lsystemc -Wl,-rpath,$(SYSTEMC_PATH)/lib

COMPILER_FLAGS_DEBUG   := -g -fsanitize=address -fsanitize=undefined -DDEBUG
COMPILER_FLAGS_RELEASE := -O2


#* Determine compiler and linker to use
COMPILER_C   := $(shell command -v gcc || command -v clang)
COMPILER_CPP := $(shell command -v g++ || command -v clang++)

ifeq ($(strip $(COMPILER_C)),)
  $(error Neither GNU's C compiler, nor LLVM's clang compiler ar available. Build failed!)
endif
ifeq ($(strip $(COMPILER_CPP)),)
  $(error Neither GNU's C++ compiler, nor LLVM's clang++ compiler ar available. Build failed!)
endif


#* Build instructions

all: debug

# Create build directory if it does not exist
mkbuilddir:
	$(shell mkdir -p $(BUILD_DIR))
	$(foreach dir,$(SRC_DIRS),$(shell mkdir -p $(BUILD_DIR)/$(dir)))

# Debug build
debug: COMPILER_FLAGS_C   += $(COMPILER_FLAGS_DEBUG)
debug: COMPILER_FLAGS_CPP += $(COMPILER_FLAGS_DEBUG)
debug: mkbuilddir $(OUTPUT)

# Release build
release: COMPILER_FLAGS_C   += $(COMPILER_FLAGS_RELEASE)
release: COMPILER_FLAGS_CPP += $(COMPILER_FLAGS_RELEASE)
release: mkbuilddir $(OUTPUT)

# Build separate object files for each source file
$(BUILD_DIR)/%.o: %.c $(HEADERS_C) $(HEADERS_CPP)
	$(COMPILER_C) $(COMPILER_FLAGS_C) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(HEADERS_C) $(HEADERS_CPP)
	$(COMPILER_CPP) $(COMPILER_FLAGS_CPP) -c $< -o $@

# Link object files into the final executable
$(OUTPUT): $(OBJECTS_C) $(OBJECTS_CPP)
	$(COMPILER_CPP) $(COMPILER_FLAGS_CPP) $(OBJECTS_C) $(OBJECTS_CPP) $(LINKER_FLAGS) -o $(OUTPUT)

# Clean up any previous build
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(OUTPUT)

.PHONY: all debug release clean
