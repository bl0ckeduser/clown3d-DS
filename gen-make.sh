# generate an ugly but functional Makefile
# for the PC target

echo "# now works in BSD \`make'

CC ?= cc"

echo "objects = " | tr -d '\n'
ls *.c | tr '\n' ' ' | sed s/\\.c/\\.o/g

echo "
"

echo "headers = " | tr -d '\n'
ls *.h | tr '\n' ' '

echo "

clown: \$(objects)
	\$(CC) -o demo \$(objects) \$(CFLAGS)

clean:
	rm -f demo \$(objects)

"

for x in `ls *.c`
do

echo "`echo $x | sed s/\\\\.c/\\\\.o/g`: $x \$(headers)
	\$(CC) -c -o `echo $x | sed s/\\\\.c/\\\\.o/g` $x \$(CFLAGS)"
done
