# Shader Collection

## Index

- [Triangle](#Triangle)
- [Mesh](#Mesh)
- [Surface](#Surface)

___

## Triangle <!-- ========================= -->

Triangle shaders to render a simple multicolored triangle.

This shader must be invoked with 3 instances since the vertices
are hardcoded in the shader and use `gl_VertexIndex` to select
one vertex and color.

#### **Input**

<span style="color:#999999">This shader takes no input.</span>

<details>
    <img src="https://www.collinsdictionary.com/images/full/cube_112883917.jpg" width=300/>
</details>

___
## Mesh <!-- ========================= -->

Basic surface shaders to render a 3D mesh

#### **Input**

- `layout(location = 0) in vec3 v_position;`
- `layout(location = 1) in vec3 v_normal;`
- `layout(location = 2) in vec3 v_color;`
- `layout(location = 3) in vec2 v_tex;`

#### **Push Constants**

- <ins>Vertex Shader</ins>
    - `layout(push_constant) uniform PVM pvm;`
        - `mat4 proj_view;`
        - `mat4 model;`

<details>
    <img src="https://www.collinsdictionary.com/images/full/cube_112883917.jpg" width=300/>
</details>

___
## Surface <!-- ========================= -->

Basic surface shaders to render a 3D object with lightning and using the PVM matrices.

This shader must be invoked with 1 instance.

#### **Input**

- `layout(location = 0) in vec3 v_position;`
- `layout(location = 1) in vec3 v_normal;`
- `layout(location = 2) in vec3 v_color;`
- `layout(location = 3) in vec2 v_tex;`

#### **Push Constants**

- <ins>Vertex Shader</ins>
    - `layout(push_constant) uniform PVM pvm;`
        - `mat4 proj_view;`
        - `mat4 model;`

#### **Uniform Buffers**

- <ins>Fragment Shader</ins>
    - `layout(binding = 0) uniform Camera cam;`
        - `vec3 position;`

<details>
    <img src="https://www.collinsdictionary.com/images/full/cube_112883917.jpg" width=300/>
</details>