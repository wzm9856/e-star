@echo off
cd ../shaders

:shaderc -f ./pbr/vs_pbr.sc -o ./pbr/vs_pbr.bin --type vertex --platform windows
:if errorlevel 1 pause:
:shaderc -f ./pbr/fs_pbr.sc -o ./pbr/fs_pbr.bin --type fragment --platform windows
:if errorlevel 1 pause:

shaderc -f ./bling-phong/vs.sc -o ./bling-phong/vs.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./bling-phong/fs_bling-phong.sc -o ./bling-phong/fs_bling-phong.bin --type fragment --platform windows
if errorlevel 1 pause:
shaderc -f ./bling-phong/fs_bling-phong_shadowMap.sc -o ./bling-phong/fs_bling-phong_shadowMap.bin --type fragment --platform windows
if errorlevel 1 pause:

shaderc -f ./ibl/vs_pbr.sc -o ./ibl/vs_pbr.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/fs_pbr.sc -o ./ibl/fs_pbr.bin --type fragment --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/fs_pbr_shadowMap.sc -o ./ibl/fs_pbr_shadowMap.bin --type fragment --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/vs_skybox.sc -o ./ibl/vs_skybox.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./ibl/fs_skybox.sc -o ./ibl/fs_skybox.bin --type fragment --platform windows
if errorlevel 1 pause:

shaderc -f ./shadowMap/ground/vs_ground.sc -o ./shadowMap/ground/vs_ground.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./shadowMap/ground/fs_ground.sc -o ./shadowMap/ground/fs_ground.bin --type fragment --platform windows
if errorlevel 1 pause:
shaderc -f ./shadowMap/shadow/vs_shadow.sc -o ./shadowMap/shadow/vs_shadow.bin --type vertex --platform windows
if errorlevel 1 pause:
shaderc -f ./shadowMap/shadow/fs_shadow.sc -o ./shadowMap/shadow/fs_shadow.bin --type fragment --platform windows
if errorlevel 1 pause:
shaderc -f ./shadowMap/test/vs_test.sc -o ./shadowMap/test/vs_test.bin --type vertex --platform windows
if errorlevel 1 pause:                                            
shaderc -f ./shadowMap/test/fs_test.sc -o ./shadowMap/test/fs_test.bin --type fragment --platform windows
if errorlevel 1 pause: