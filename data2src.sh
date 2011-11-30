# data2src.sh source dest array-name
# must manually prune garbage on last line
# of dest !

cp data2src-c.txt d2src-temp.c
gcc d2src-temp.c -o data2src
echo "/*" > $2
cat d2src-temp.c >> $2
echo "*/" >> $2
cat $1 | ./data2src $3 >> $2
rm data2src
rm d2src-temp.c

echo "converted."

