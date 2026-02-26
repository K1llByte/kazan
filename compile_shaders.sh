#!/bin/bash

BOLD_GREEN="\033[1;32m"
RESET="\033[0m"

compile_glsl_shaders_by_ext() {
    for ext in "$@"; do
        for filename in assets/shaders/**/*.${ext}; do
            echo -e "${BOLD_GREEN}Compiling GLSL ${filename}${RESET}"
            glslangValidator -V "${filename}" -o "${filename}.spv"
        done
    done
}

compile_slang_shaders_by_ext() {
    for ext in "$@"; do
        for filename in assets/shaders/**/*.${ext}; do
            echo -e "${BOLD_GREEN}Compiling Slang ${filename}${RESET}"
            slangc "${filename}" -target spirv -profile sm_6_5 -o "${filename}.spv"
        done
    done
}


shopt -s globstar nullglob

# Compile glsl shaders
compile_glsl_shaders_by_ext "vert" "vs"
compile_glsl_shaders_by_ext "frag" "fs"
compile_glsl_shaders_by_ext "geom" "gs"
compile_glsl_shaders_by_ext "tesc" "tcs"
compile_glsl_shaders_by_ext "tese" "tes"
compile_glsl_shaders_by_ext "comp" "cs"
compile_glsl_shaders_by_ext "rgen"
compile_glsl_shaders_by_ext "rint"
compile_glsl_shaders_by_ext "rahit"
compile_glsl_shaders_by_ext "rchit"
compile_glsl_shaders_by_ext "rmiss"
compile_glsl_shaders_by_ext "rcall"
compile_glsl_shaders_by_ext "task"
compile_glsl_shaders_by_ext "mesh"

compile_slang_shaders_by_ext "slang"