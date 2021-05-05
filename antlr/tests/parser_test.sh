mkdir testing
cp ../Expr.g4 ./testing
cp ../tests/Makefile ./testing
cp -r ../../task1 ./testing
cd testing
make

grun='java org.antlr.v4.gui.TestRig'
flag=0

for file in `ls task1`
do 
    echo $file
    $grun Expr prog task1/$file 2> op
    if [[ $(cat op) ]]
    then
        cat op
        flag=1
    fi
done

rm op
make clean
cd ..
rm -rf testing
exit $flag