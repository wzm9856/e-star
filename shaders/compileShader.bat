D:
cd D:\Projects\project-e-star-homework\shaders
shaderc -f ./pbr/vs.sc -o ./pbr/vs.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./pbr/fs_pbr.sc -o ./pbr/fs_pbr.bin --type fragment --platform windows
if errorlevel 1 pause:

shaderc -f ./bling-phong/vs.sc -o ./bling-phong/vs.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./bling-phong/fs_bling-phong.sc -o ./bling-phong/fs_bling-phong.bin --type fragment --platform windows
if errorlevel 1 pause:

shaderc -f ./ibl/vs.sc -o ./ibl/vs.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/fs_pbr.sc -o ./ibl/fs_pbr.bin --type fragment --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/vs_skybox.sc -o ./ibl/vs_skybox.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/fs_skybox.sc -o ./ibl/fs_skybox.bin --type fragment --platform windows
if errorlevel 1 pause:
