#!/bin/bash

if [[ ! -z $1 ]]; then
    glslangValidator -V $1 -o $1.spv
else
    shopt -s globstar

    for filename in assets/shaders/**/*.vert; do
    glslangValidator -V $filename -o $filename.spv
    done
    for filename in assets/shaders/**/*.frag; do
        glslangValidator -V $filename -o $filename.spv
    done
fi

