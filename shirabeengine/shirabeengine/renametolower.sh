# find . -depth  -exec sh -c '
#     t=${0%/*}/$(printf %s "${0##*/}" | tr "[:upper:]" "[:lower:]");
#     [ "$t" = "$0" ] && [ "$t" != "CMakeLists.txt" ] || mv -i "$0" "$t"
# ' {} \;
# find . -depth -name 'cmakelists.txt' -exec sh -c '
#     t=${0%}
#     [ "$t" = "$0" ] || mv -i "$t" "CMakeLists.txt"
# ' {} \;

VALUES=$(find ${PWD}/modules_back/ -name "CMakeLists.txt")
for value in ${VALUES[*]} 
do
    TVALUE=${value/modules_back/modules}
    echo "cp $value -> $TVALUE"
    cp $value $TVALUE
    
done
