#! /bin/sh

echo "#ifndef rename_h_as7d46as4d" > rename.h
echo "#define rename_h_as7d46as4d" >> rename.h

cat pcre.h | grep PCRE_EXP_DECL | grep -v '#' | grep '(' | sed -e 's/(\*//' | sed -e 's/[()].*//' | sed -e 's/.*[ *]//' | sort | uniq | while read l; do echo "#define $l internal_$l"; done >> rename.h

cat pcreposix.h | grep PCREPOSIX_EXP_DECL | grep -v '#' | grep '(' | sed -e 's/(\*//' | sed -e 's/[()].*//' | sed -e 's/.*[ *]//' | sort | uniq | while read l; do echo "#define $l internal_$l"; done >> rename.h

echo "#endif" >> rename.h
