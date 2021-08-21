CC = g++ # clang++
INCLUDE = -I include/ -I include/lib/
WARNIGNS = -Wall -Wextra -Wshadow -Wfatal-errors -pedantic
FLAGS = $(WARNIGNS) -std=c++17 -O2 $(INCLUDE) # -fsanitize=address
LIB_FLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
SRC_EXT = cpp

GLSLC = glslangValidator # glslc
VERT_EXT = vert
FRAG_EXT = frag

#############################################################

BUILD_DIR = build
SRC_DIR = src
TARGET = bin/renderer

SHADER_DIR = shaders

########################### All #############################

all: renderer

######################### Renderer ##########################

SRCS = $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SRCS:.$(SRC_EXT)=.o))

renderer: renderer_directories $(TARGET)

$(TARGET): $(OBJS) 
	$(CC) $^ -o $(TARGET) $(LIB_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	$(CC) $(FLAGS) -c -o $@ $<

########################## Shaders ##########################

VERTS = $(shell find $(SHADER_DIR) -type f -name *.$(VERT_EXT))
VERTS_SPV = $(patsubst $(SHADER_DIR)/%,$(SHADER_DIR)/%,$(VERTS:.$(VERT_EXT)=.$(VERT_EXT).spv))

FRAGS = $(shell find $(SHADER_DIR) -type f -name *.$(FRAG_EXT))
FRAGS_SPV = $(patsubst $(SHADER_DIR)/%,$(SHADER_DIR)/%,$(FRAGS:.$(FRAG_EXT)=.$(FRAG_EXT).spv))

shaders: $(VERTS_SPV) $(FRAGS_SPV)

# # GLSLC
# $(SHADER_DIR)/%.$(VERT_EXT).spv: $(SHADER_DIR)/%.$(VERT_EXT)
# 	$(GLSLC) $< -o $@

# $(SHADER_DIR)/%.$(FRAG_EXT).spv: $(SHADER_DIR)/%.$(FRAG_EXT)
# 	$(GLSLC) $< -o $@

# GLSLang
$(SHADER_DIR)/%.$(VERT_EXT).spv: $(SHADER_DIR)/%.$(VERT_EXT)
	$(GLSLC) -V $< -o $@

$(SHADER_DIR)/%.$(FRAG_EXT).spv: $(SHADER_DIR)/%.$(FRAG_EXT)
	$(GLSLC) -V $< -o $@

#############################################################

renderer_directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p bin

clean:
	rm -rf $(BUILD_DIR) bin/*

########################### Tests ###########################

run: all
	$(TARGET)

main:
	$(CC) src/main.cpp -o $(TARGET) $(LIB_FLAGS)

#############################################################