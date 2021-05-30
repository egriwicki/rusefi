#!/usr/bin/env bash

DIR=$(cd -P -- "$(dirname -- "$0")" && pwd -P)"/"
TEXT=$(sed -e "/###CSS###/{r ${DIR}style.css" -e 'd}' -e "/###JS###/{r ${DIR}script.js" -e 'd}' ${DIR}pinout.html | sed -e "/\/\/\/DATA\/\/\//{a `${1//\//\\/}\`,\n///DATA///" -e "d}")
echo "$TEXT" > $2
