*** ABOUT ***

DNG for PowerShot-2 (DNG4PS-2) is free software for converting RAW files from 
Canon PowerShot cameras with Digic-II processor and modified firmware into 
Digital Negative (DNG) format. 

*** BUILDING FROM SOURCES ***

There are 3 separate projects in sources:

  DNG4PS-2.cbp - for MS Windows
  dng4ps2li.cbp - For Linux
  src/Makefile - For Linux

For the first two projects the following software must be installed to build:

  1. wxWidgets 2.9.x library (http://www.wxwidgets.org/).

  2. Boost library (http://www.boost.org/)

  3. RAD tool "Code::Blocks" (http://www.codeblocks.org/). Newest nightly build
     are required

  4. If you want to compile under MS Windows, install MinGW (CGG compiler for 
     Windows) with gettext module (http://www.mingw.org/).

If compiling via Makefile the following is required:

  1. wxWidgets 2.9.x library (http://www.wxwidgets.org/).

  2. g++ >= 4.8
  
  3. development headers for libc, zlib, libwxgtk, libexif, libexpat1
  
  4. gettext utilties

  eg. make -C src ARCH=64

*** LINKS ***

Project's web page: http://code.google.com/p/dng4ps2
Author's email:     denis.artyomov@gmail.com
Developer's email:  glenn.washburn@efficientek.com
git:                https://github.com/crass/dng4ps2
PPA:                https://code.launchpad.net/~crass/+archive/dng4ps2

