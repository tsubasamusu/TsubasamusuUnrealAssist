@echo off
echo =====================================================
echo Deleting Build Folders and .sln files
echo =====================================================

REM プロジェクトルートディレクトリのフォルダを削除
if exist "..\..\Binaries" (
    echo Removing Binaries...
    rmdir /s /q "..\..\Binaries"
)
if exist "..\..\Build" (
    echo Removing Build...
    rmdir /s /q "..\..\Build"
)
if exist "..\..\DerivedDataCache" (
    echo Removing DerivedDataCache...
    rmdir /s /q "..\..\DerivedDataCache"
)
if exist "..\..\Intermediate" (
    echo Removing Intermediate...
    rmdir /s /q "..\..\Intermediate"
)

REM プロジェクトルートディレクトリの .sln ファイルを削除
pushd "..\..\"
for %%f in (*.sln) do (
    echo Deleting %%f...
    del /q "%%f"
)
popd

REM このプラグイン自身のフォルダ削除
if exist "Binaries" (
    echo Removing Plugins\TsubasamusuUnrealAssist\Binaries...
    rmdir /s /q "Binaries"
)
if exist "Intermediate" (
    echo Removing Plugins\TsubasamusuUnrealAssist\Intermediate...
    rmdir /s /q "Intermediate"
)

echo =====================================================
echo Done.
echo =====================================================
pause