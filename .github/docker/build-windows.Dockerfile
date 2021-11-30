# escape=`

# Copyright (C) 2020 Intel Corporation
# SPDX-License-Identifier: MIT

FROM mcr.microsoft.com/dotnet/framework/sdk:4.7.2-20190312-windowsservercore-ltsc2016 AS toolchain

SHELL ["powershell"]

# Install Visual Studio
ARG VS_VERSION=16
ENV VS_VERSION $VS_VERSION
ARG VS_EDITION
ENV VS_EDITION $VS_EDITION
RUN mkdir C:\TEMP; `
    wget `
      -Uri https://aka.ms/vs/$env:VS_VERSION/release/vs_$env:VS_EDITION.exe `
      -OutFile C:\TEMP\vs_$env:VS_EDITION.exe
SHELL ["cmd", "/S", "/C"]
ARG VS_PRODUCT_KEY=" "
ENV VS_PRODUCT_KEY $VS_PRODUCT_KEY
RUN C:\TEMP\vs_%VS_EDITION%.exe %VS_PRODUCT_KEY% `
      --quiet --wait --norestart --nocache `
      --installPath C:\VS `
      --includeRecommended `
      --add Microsoft.VisualStudio.Workload.NativeDesktop `
      --add Microsoft.VisualStudio.Workload.ManagedDesktop `
      --add Microsoft.VisualStudio.Component.VC.Runtimes.x86.x64.Spectre `
      --add Microsoft.VisualStudio.Component.Git `
    || IF "%ERRORLEVEL%"=="3010" EXIT 0

SHELL ["powershell"]

ENV chocolateyVersion "0.10.15"
RUN [System.Net.ServicePointManager]::SecurityProtocol = 3072 -bor 768 -bor 192 -bor 48; `
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1')); `
    wget `
      -Uri https://github.com/frerich/clcache/releases/download/v4.2.0/clcache.4.2.0.nupkg `
      -OutFile clcache.4.2.0.nupkg `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    choco install -y --no-progress --fail-on-error-output 7zip; `
    choco install -y --no-progress --fail-on-error-output clcache --source=.

# cpack (an alias from chocolatey) and cmake's cpack conflict.
RUN Remove-Item -Force 'C:\ProgramData\chocolatey\bin\cpack.exe'

FROM toolchain AS toolchain_deps_zlib

# install zlib
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://zlib.net/zlib1211.zip `
      -OutFile zlib1211.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x zlib1211.zip; `
    rm zlib1211.zip
SHELL ["cmd", "/S", "/C"]
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\zlib-1.2.11 && `
    mkdir build && `
    cd build && `
    cmake .. -A x64 -T host=x64 `
      -D BUILD_SHARED_LIBS=YES `
      -D CMAKE_INSTALL_PREFIX:PATH=/zlib && `
    cmake --build . --target INSTALL --config Release
ENV CMAKE_PREFIX_PATH /zlib

FROM toolchain_deps_zlib AS toolchain_deps_libpng

# install libpng
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://prdownloads.sourceforge.net/libpng/lpng1637.zip `
      -OutFile lpng1637.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x lpng1637.zip; `
    rm lpng1637.zip
SHELL ["cmd", "/S", "/C"]
# WIP: find out how to get these variables into the docker environment, so we don't have to call this ourselves at runtime (tricky!!)
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\lpng1637 && `
    mkdir build && `
    cd build && `
    cmake .. -T host=x64 -A x64 -D CMAKE_INSTALL_PREFIX:PATH=/libpng && `
    cmake --build . --target INSTALL --config Release

FROM toolchain AS toolchain_deps_opencl

# install OpenCL headers
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri "https://github.com/KhronosGroup/OpenCL-Headers/archive/de26592167b9fdea503885e40e8755393c56523d.zip" `
      -OutFile OpenCL-Headers.zip; `
    . 'C:\Program Files\7-Zip\7z.exe' x OpenCL-Headers.zip; `
    rm OpenCL-Headers.zip;
RUN mkdir /opencl/include -Force | Out-Null; `
    mv C:\TEMP\OpenCL-Headers-de26592167b9fdea503885e40e8755393c56523d\CL /opencl/include/CL;

# install OpenCL ICD Loader
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://github.com/KhronosGroup/OpenCL-ICD-Loader/archive/b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip `
      -OutFile OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip; `
    rm OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86.zip
SHELL ["cmd", "/S", "/C"]
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    cd C:\TEMP\OpenCL-ICD-Loader-b342ff7b7f70a4b3f2cfc53215af8fa20adc3d86 && `
    mkdir build && `
    cd build && `
    cmake .. `
      -T host=x64 `
      -A x64 `
      -D OPENCL_INCLUDE_DIRS="/opencl/include" `
      -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY="/opencl/lib" `
      -D CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE="/opencl/lib" && `
    cmake --build . --config Release

FROM toolchain AS toolchain_deps_boost

# install boost
SHELL ["powershell"]
RUN [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; `
    cd C:\TEMP; `
    wget `
      -Uri https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip `
      -OutFile boost_1_70_0.zip `
      -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::InternetExplorer; `
    . 'C:\Program Files\7-Zip\7z.exe' x boost_1_70_0.zip; `
    rm boost_1_70_0.zip
SHELL ["cmd", "/S", "/C"]
RUN cd C:\VS\Common7\Tools & VsDevCmd && `
    call "%VCInstallDir%\Auxiliary\Build\vcvars64.bat" && `
    cd C:\TEMP\boost_1_70_0 && `
    .\bootstrap && `
    .\b2 install `
      -j 4 `
      address-model=64 `
      --prefix=/boost `
      --with-chrono `
      --with-log `
      --with-program_options `
      --with-serialization `
      --with-system `
      --with-timer

FROM toolchain AS dev

COPY --from=toolchain_deps_boost ["/boost", "/boost"]
COPY --from=toolchain_deps_zlib ["/zlib", "/zlib"]
COPY --from=toolchain_deps_libpng ["/libpng", "/libpng"]
COPY --from=toolchain_deps_opencl ["/opencl", "/opencl"]

ENV CMAKE_PREFIX_PATH "/boost;/zlib;/libpng;/opencl"

SHELL ["cmd", "/S", "/C"]

ENTRYPOINT ["C:\\VS\\Common7\\Tools\\VsDevCmd.bat", "-arch=amd64", "-host_arch=amd64", "&&", "cmd", "/S", "/C"]
