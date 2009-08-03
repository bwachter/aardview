@echo off
set buildmode=debug
set clickonce=c:\clickonce
set version=0.0.0.1
set cwd=%CD%
set providerurl=http://bwachter.lart.info/projects/aardview/aardview.application
set bindir=%version%\bin\

if not exist aardview\%buildmode%\aardview.exe goto end
if not exist %clickonce%\aardview\nul goto end
if not exist %clickonce%\aardview\%version%\nul mkdir %clickonce%\aardview\%version% || goto end
if not exist %clickonce%\aardview\%version%\bin\nul mkdir %clickonce%\aardview\%bindir% || goto end

copy aardview\%buildmode%\aardview.exe %clickonce%\aardview\%bindir% || goto end

cd cowrap || goto end
msbuild /p:Configuration=Release || goto end
if not exist cowrap\obj\Release\cowrap.exe goto end
copy cowrap\obj\Release\cowrap.exe %clickonce%\aardview\%bindir% || goto end

cd %clickonce%\aardview || goto end
mage -New Application -p x86 -tr FullTrust -ToFile %version%\aardview.exe.manifest -name "Aardview" -Version %version% -FromDirectory %bindir% || goto end
mage -New Deployment -p x86 -i t -ToFile aardview.application -Name "Aardview" -Version %version% -AppManifest %version%\aardview.exe.manifest -providerUrl %providerurl% || goto end

echo You should find the deployment files in %clickonce%\aardview\
goto done

:end
echo Something didn't work out, sorry.

:done
cd %cwd%