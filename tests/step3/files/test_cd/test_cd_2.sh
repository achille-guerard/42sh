echo $PWD
ln -s ../../step2 step2_syml #create a simlink
echo $PWD
cd step2_syml/files
echo $PWD
cd ../../
echo $PWD
rm step2_syml
