#!/bin/bash
function runConfig(){
echo "Running $1"
if [ -d pdf ] ; then
 echo "Removing working directory pdf"
fi
mkdir pdf
/home/alice/trigger/rl/source/extract $1 > pdf/out.log
mv pdf last/$1
}
if [ $# = 1 ] ;then
 echo "RUN " $1 "is being processed"
 if [ -d  last ] ; then
    tar -cvzf run$1.tgz last/
    mv run$1.tgz archive/.
    rm -r last > /dev/null
 fi
 ls -1 /data/alice/trigger/SMAQ/testb/ > files.txt
 mkdir last
 cp files.txt last/.
 #runConfig confMuon
 #runConfig confMinBias
 #runConfig confCals
 runConfig config
 if [ -d last ] ;then 
  echo "Results are in directory last"
  rm files.txt
  mv report.txt last/
  cat last/report.txt 
  exit 1
 fi
else
 echo "One parameter - RUN number - expected."
fi
