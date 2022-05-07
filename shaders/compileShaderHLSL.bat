@echo off
D:
cd D:\Projects\project-e-star-homework\shaders

:shaderc -f ./pbr/vs_pbr.sc -o ./pbr/vs_pbr_hlsl.bin --type vertex --platform windows -p s_5_0
:if errorlevel 1 pause:
:shaderc -f ./pbr/fs_pbr.sc -o ./pbr/fs_pbr_hlsl.bin --type fragment --platform windows -p s_5_0
:if errorlevel 1 pause:

shaderc -f ./bling-phong/vs.sc -o ./bling-phong/vs_hlsl.bin --type vertex --platform windows --profile vs_5_0
if errorlevel 1 pause:
shaderc -f ./bling-phong/fs_bling-phong.sc -o ./bling-phong/fs_bling-phong_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:
shaderc -f ./bling-phong/fs_bling-phong_shadowMap.sc -o ./bling-phong/fs_bling-phong_shadowMap_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:

shaderc -f ./ibl/vs_pbr.sc -o ./ibl/vs_pbr_hlsl.bin --type vertex --platform windows -p vs_5_0
if errorlevel 1 pause:
shaderc -f ./ibl/fs_pbr.sc -o ./ibl/fs_pbr_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:
shaderc -f ./ibl/fs_pbr_shadowMap.sc -o ./ibl/fs_pbr_shadowMap_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:
shaderc -f ./ibl/vs_skybox.sc -o ./ibl/vs_skybox_hlsl.bin --type vertex --platform windows -p vs_5_0
if errorlevel 1 pause:
shaderc -f ./ibl/fs_skybox.sc -o ./ibl/fs_skybox_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:

shaderc -f ./shadowMap/ground/vs_ground.sc -o ./shadowMap/ground/vs_ground_hlsl.bin --type vertex --platform windows -p vs_5_0
if errorlevel 1 pause:
shaderc -f ./shadowMap/ground/fs_ground.sc -o ./shadowMap/ground/fs_ground_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:
shaderc -f ./shadowMap/shadow/vs_shadow.sc -o ./shadowMap/shadow/vs_shadow_hlsl.bin --type vertex --platform windows -p vs_5_0
if errorlevel 1 pause:
shaderc -f ./shadowMap/shadow/fs_shadow.sc -o ./shadowMap/shadow/fs_shadow_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause:
shaderc -f ./shadowMap/test/vs_test.sc -o ./shadowMap/test/vs_test_hlsl.bin --type vertex --platform windows -p vs_5_0
if errorlevel 1 pause:                                            
shaderc -f ./shadowMap/test/fs_test.sc -o ./shadowMap/test/fs_test_hlsl.bin --type fragment --platform windows -p ps_5_0
if errorlevel 1 pause: