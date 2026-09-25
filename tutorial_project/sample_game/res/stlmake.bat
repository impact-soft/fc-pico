::/ @file stlmake.bat
::/ @brief Turns one STL mesh into the C arrays the renderer uses.
::/ @ingroup toolchain
::/
::/ `stl_maker` reduces the mesh and `stl_converter` emits the vertex array and,
::/ where there is one, the collision table.
..\..\bin\stl_maker %1 %1
..\..\bin\stl_converter %1
