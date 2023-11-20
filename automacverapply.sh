#!/bin/bash
# Automatic version updater for Mac OS X from res/resource.h
# Script by rageworx@gmail.com

VER_GREP=`grep APP_VERSION_STR res/resource.h`
VER_ARR=($VER_GREP)
VER_STR=`echo ${VER_ARR[2]} | sed "s/\"//g"`

APP_NAME="$1"
IP_SRC="bin/${APP_NAME}/Contents/Info.plist"

TEST_ARR=`echo ${VER_STR} | tr . '\n' | awk '{ print $1 }'`
SVER_ARR=(${TEST_ARR})
SVER_STR="${SVER_ARR[0]}.${SVER_ARR[1]}.${SVER_ARR[2]}"
BUILD_STR="${SVER_ARR[3]}"

#Shows versions
echo "Version : ${SVER_STR}(${BUILD_STR})"

#Ok, prorcessing now.
STR_VLONG="--VER_LONG--"
STR_VSHORT="--VER_SHORT--"
TMPF1="_tmpf_"
if [ -f ${IP_SRC} ]; then
	echo "Applying version to ${IP_SRC} ..."
	sed "s/${STR_VLONG}/${BUILD_STR}/g" ${IP_SRC} > ${TMPF1}
	sed "s/${STR_VSHORT}/${SVER_STR}/g" ${TMPF1} > ${IP_SRC}
	rm -rf ${TMPF1}
	echo "Completed."
else
	echo "Error: ${IP_SRC} not found."
fi
