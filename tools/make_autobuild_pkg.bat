set SRC_DIR="%USERPROFILE%\Desktop\cef_binary_3.2526.1373.gb660893_windows32"
set DST_DIR="%USERPROFILE%\Desktop\cef_2526_WIN_32"

if not exist %SRC_DIR% goto end
if not exist %DST_DIR% goto end

mkdir %DST_DIR%"\bin\release"
mkdir %DST_DIR%"\include"
mkdir %DST_DIR%"\lib\debug"
mkdir %DST_DIR%"\lib\release"
mkdir %DST_DIR%"\resources"

rem ******** bin folder ********
copy %SRC_DIR%"\Release\d3dcompiler_43.dll" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\d3dcompiler_47.dll" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\libcef.dll" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\libEGL.dll" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\libGLESv2.dll" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\natives_blob.bin" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\snapshot_blob.bin" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\widevinecdmadapter.dll" %DST_DIR%"\bin\release"
copy %SRC_DIR%"\Release\wow_helper.exe" %DST_DIR%"\bin\release"

rem ******** bin include ********
xcopy %SRC_DIR%"\include\*" %DST_DIR%"\include\" /S

rem ******** lib folder ********
copy %SRC_DIR%"\Debug\libcef.lib" %DST_DIR%"\lib\debug"
copy %SRC_DIR%"\build\libcef_dll_wrapper\Debug\libcef_dll_wrapper.lib" %DST_DIR%"\lib\debug"
copy %SRC_DIR%"\Release\libcef.lib" %DST_DIR%"\lib\release"
copy %SRC_DIR%"\build\libcef_dll_wrapper\Release\libcef_dll_wrapper.lib" %DST_DIR%"\lib\release"

rem ******** resources folder ********
xcopy %SRC_DIR%"\resources\*" %DST_DIR%"\resources\" /S

:END
