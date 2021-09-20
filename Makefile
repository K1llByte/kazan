CC = g++ # clang++
LIB_INCLUDE = $(patsubst %, -I %, $(LIB_INCLUDE_DIRS))
INCLUDE = -I include/ $(LIB_INCLUDE)
WARNIGNS = -Wall -Wextra -Wshadow -Wfatal-errors -pedantic
FLAGS = $(WARNIGNS) -std=c++2a -O2 $(INCLUDE) # -fsanitize=address
LIB_FLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
SRC_EXT = cpp

# GLSLC = glslc
GLSLC = glslangValidator
VERT_EXT = vert
FRAG_EXT = frag

#############################################################

BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
LIB_INCLUDE_DIRS = $(shell find $(INCLUDE_DIR)/lib/* -maxdepth 1 -type d)
LIB_BUILD_DIRS = $(patsubst $(INCLUDE_DIR)/%, $(BUILD_DIR)/%, $(LIB_INCLUDE_DIRS))
TARGET = bin/renderer

SHADER_DIR = shaders

########################### All #############################

all: renderer

########################### Libs ############################

LIB_SRCS = $(shell find $(SRC_DIR)/lib -type f -name *.$(SRC_EXT))
LIB_OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(LIB_SRCS:.$(SRC_EXT)=.o))

$(BUILD_DIR)/lib/%.o: $(SRC_DIR)/lib/%.$(SRC_EXT)
	$(CC) $(FLAGS) -c -o $@ $<

######################### Renderer ##########################

SRCS = $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.$(SRC_EXT)=.o))

renderer: renderer_directories $(TARGET)

$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $^ -o $(TARGET) $(LIB_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	$(CC) $(FLAGS) -c -o $@ $<

########################## Shaders ##########################

VERTS = $(shell find $(SHADER_DIR) -type f -name *.$(VERT_EXT))
VERTS_SPV = $(patsubst $(SHADER_DIR)/%,$(SHADER_DIR)/%,$(VERTS:.$(VERT_EXT)=.$(VERT_EXT).spv))

FRAGS = $(shell find $(SHADER_DIR) -type f -name *.$(FRAG_EXT))
FRAGS_SPV = $(patsubst $(SHADER_DIR)/%,$(SHADER_DIR)/%,$(FRAGS:.$(FRAG_EXT)=.$(FRAG_EXT).spv))

shaders: $(VERTS_SPV) $(FRAGS_SPV)


ifeq ($(GLSLC), glslc)
# GLSLC
$(SHADER_DIR)/%.$(VERT_EXT).spv: $(SHADER_DIR)/%.$(VERT_EXT)
	$(GLSLC) $< -o $@

$(SHADER_DIR)/%.$(FRAG_EXT).spv: $(SHADER_DIR)/%.$(FRAG_EXT)
	$(GLSLC) $< -o $@

else
# GLSLang
$(SHADER_DIR)/%.$(VERT_EXT).spv: $(SHADER_DIR)/%.$(VERT_EXT)
	$(GLSLC) -V $< -o $@

$(SHADER_DIR)/%.$(FRAG_EXT).spv: $(SHADER_DIR)/%.$(FRAG_EXT)
	$(GLSLC) -V $< -o $@

endif

#############################################################

renderer_directories:
	@mkdir -p $(LIB_BUILD_DIRS)
	@mkdir -p $(BUILD_DIR)
	@mkdir -p bin

clean:
	rm -rf $(BUILD_DIR)/*.o bin/*
# rm -rf $(BUILD_DIR) bin/*

########################### Tests ###########################

run: all
	$(TARGET)

main:
	$(CC) src/main.cpp -o $(TARGET) $(LIB_FLAGS)

#############################################################