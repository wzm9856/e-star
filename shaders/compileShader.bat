D:
cd D:\Projects\project-e-star-homework\shaders
shaderc -f ./fs_cubes.sc -o ./fs_cubes.bin --type fragment --platform windows
shaderc -f ./vs_cubes.sc -o ./vs_cubes.bin --type vertex --platform windows
pause: