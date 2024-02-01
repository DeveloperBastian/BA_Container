@echo off
d:
cd\
cd "D:\3D\Unreal_Projects\BA_Containers"
rmdir /s /q .vs
rmdir /s /q Binaries
rmdir /s /q DerivedDataCache
rmdir /s /q Intermediate
rmdir /s /q Saved
del *.sln
cd "D:\3D\Unreal_Projects\BA_Containers\Plugins\Containers"
rmdir /s /q Binaries
rmdir /s /q Intermediate
