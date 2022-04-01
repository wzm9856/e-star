D:
cd D:\Projects\project-e-star-homework\shaders
shaderc -f ./vs.sc -o ./vs.bin --type vertex --platform windows
shaderc -f ./fs_bling-phong.sc -o ./fs_bling-phong.bin --type fragment --platform windows
shaderc -f ./fs_pbr.sc -o ./fs_pbr.bin --type fragment --platform windows

pause: