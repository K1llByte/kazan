#!/bin/bash

BOLD_GREEN="\033[1;32m"
BOLD_RED="\033[1;31m"
RESET="\033[0m"

compile_glsl_shaders_by_ext() {
    if ! command -v glslangValidator >/dev/null 2>&1; then
        echo -e "${BOLD_RED}Error:${RESET} Command glslangValidator does not exist"
        return 1
    fi

    for ext in "$@"; do
        for filename in assets/shaders/**/*.${ext}; do
            # NOTE: When compiling with -Os unused dsets will be stripped out
            # of the shader, and therefore wont be visible by shader reflection
            if glslangValidator -V "$filename" -o "$filename.spv" ; then
                echo -e "${BOLD_GREEN}Compiled${RESET} ${filename}"
            fi
        done
    done
}

compile_slang_shaders_by_ext() {
    if ! command -v slangc >/dev/null 2>&1; then
        echo -e "${BOLD_RED}Error:${RESET} Command slangc does not exist"
        return 1
    fi

    for ext in "$@"; do
        for filename in assets/shaders/**/*.${ext}; do
            if slangc "${filename}" -target spirv -profile sm_6_5 -o "${filename}.spv" > /dev/null; then
                echo -e "${BOLD_GREEN}Compiled${RESET} ${filename}"
            fi
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
# Compile slang shaders
compile_slang_shaders_by_ext "slang"