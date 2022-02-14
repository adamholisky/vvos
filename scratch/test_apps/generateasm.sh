#!/bin/bash
SYM_PREFIX="vvs_app_"

rm -f build_temp/*.s

FILES="*.c"

for F in $FILES
do
	F="$(basename -s .c $F)"

	SYM="${SYM_PREFIX}${F}"
	echo "Processing $F"

	#.section .rawdata
    #.incbin "../../build/alpha.vvs"
	printf ".section .$SYM\n.incbin \"build_objout/$F.vvs\"\n" > build_temp/$F.s
done