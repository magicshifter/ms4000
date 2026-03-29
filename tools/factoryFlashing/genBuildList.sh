#!/bin/sh
@echo "generating issueUpdate list for incorporation into MS3000Tool.py"
find ../../web/app/build/ -type file -exec echo issueUploadMS3000\(ser, \"{} {} \;
