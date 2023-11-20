#!/bin/bash
# A configuring script for freetype2.
# Use this script to configure freetype2 for macOS.

CFGS+="--enable-static=yes "
CFGS+="--enable-shared=no "
CFGS+="--with-png=no "
CFGS+="--with-bzip2=no "
CFGS+="--with-zlib=no "
CFGS+="--with-harfbuzz=no "
CFGS+="--with-brotli=no "
CFGS+="--with-librsvg=no "

./configure CFLAGS="-arch arm64 -arch x86_64 -mmacosx-version-min=11.0" ${CFGS}
