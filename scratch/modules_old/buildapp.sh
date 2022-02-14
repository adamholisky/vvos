#!/bin/bash

SYM_PREFIX="vvs_app_"
SYM_POSTFIX_START="_start"
SYM_POSTFIX_END="_end"

cd build
rm ../build_temp/apps_insert.ld
touch ../build_temp/apps_insert.ld

FILES="*.vvs"


for F in $FILES
do
	F="$(basename -s .vvs $F)"

	SYM="${SYM_PREFIX}${F}"
	SYM_START="${SYM}${SYM_POSTFIX_START}"
	SYM_END="${SYM}${SYM_POSTFIX_END}"
	echo "Processing $F"

	printf ".$SYM_START = .;\n*(.$SYM*)\n.$SYM_END = .;\n" >> ../build_temp/apps_insert.ld
done