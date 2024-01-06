@echo off
glslc.exe Shader/Vertex.vert -o Shader/Vertex.spv
glslc.exe Shader/Fragment.frag -o Shader/Fragment.spv

glslc.exe Shader/OffScreenVertex.vert -o Shader/OffScreenVertex.spv
glslc.exe Shader/OffScreenFragment.frag -o Shader/OffScreenFragment.spv

glslc.exe Shader/GUIVertex.vert -o Shader/GUIVertex.spv
glslc.exe Shader/GUIFragment.frag -o Shader/GUIFragment.spv

glslc.exe Shader/BlurVertex.vert -o Shader/BlurVertex.spv
glslc.exe Shader/BlurFragment.frag -o Shader/BlurFragment.spv

glslc.exe Shader/SkyboxVertex.vert -o Shader/SkyboxVertex.spv
glslc.exe Shader/SkyboxFragment.frag -o Shader/SkyboxFragment.spv

pause