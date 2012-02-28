#! /bin/sh

rm CMakeLists.inc
rm quicklz.inc
rm tables2.inc

echo "{" > tables1.inc

for v in 1.31 1.40; do
    echo "{" >> tables1.inc
    V=`echo "$v" | sed -e 's/\./_/'`
    for i in 0 1 2 3; do
        echo "{" >> tables1.inc
        for j in 0 100000 1000000; do
            c="$V"_"$i"_"$j".c
            h="$V"_"$i"_"$j".h

            rm "$c"
            rm "$h"

            if test ! "$V"_"$i" = "1_40_0"; then
                echo "#define FROM_QUICKLZ_BUILD" >> "$c"
                echo "#include \"$h\"" >> "$c"
                echo "#include \"$v/quicklz.c\"" >> "$c"
                echo "#include \"table.inc\"" >> "$c"

                echo "#define COMPRESSION_LEVEL $i" >> "$h"
                echo "#define STREAMING_MODE    $j" >> "$h"
                echo "#define QLZ_YVERSION      $V" >> "$h"
                echo "" >> "$h"
                echo "#include \"prolog.h\"" >> "$h"
                echo "#include \"$v/yquicklz.h\"" >> "$h"
                echo "#include \"epilog.h\"" >> "$h"

                echo "SRCS($c)" >> CMakeLists.inc

                echo "#include \"$h\"" >> quicklz.inc

                echo "&yqlz"_"$V"_"$i"_"$j"_"table," >> tables1.inc
                echo "extern struct TQuickLZMethods yqlz"_"$V"_"$i"_"$j"_"table;" >> tables2.inc
            else
                echo "0," >> tables1.inc
            fi
        done
        echo "}," >> tables1.inc
    done
    echo "}," >> tables1.inc
done

echo "}" >> tables1.inc
cat tables1.inc | tr -d '\n' > tmp
cat tmp | sed -e 's/\,\}/\}/g' > tables1.inc
rm tmp
