
maum: maum.c maum.h util.c util.h
	gcc -g -Wall -o maum maum.c util.c

clean:
	rm -f maum
