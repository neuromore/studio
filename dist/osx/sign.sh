#!/bin/bash
# This requires the p12 neuromore code signing file to be imported on your mac
# Must also delete 'Studio.app/Contents/MacOS/.gitignore'

codesign -a x86_64 --deep --verbose --force --sign "neuromore co" ./Studio.app
codesign -v ./Studio.app
