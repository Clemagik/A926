#!/bin/bash

sed -e 's/#!\/bin\/bash/@echo off/' -e 's/export /set /g' -e 's/`pwd`/%~dp0/g' -e 's/\//\\/g' -e 's/\$/\%/' -e 's/%SDK_ROOT/%SDK_ROOT%/' build/core/tools/sdk-build > ./build/core/tools/sdk-build.cmd

chmod +x ./build/core/tools/sdk-build.cmd
