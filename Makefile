all: dupes.c
	${RM} ${HOME}/local/bin/dupes
	gcc -O3 dupes.c -o ${HOME}/local/bin/dupes -I.
	

clean:
	$(RM) dupes
	$(RM) ${HOME}/local/bin/dupes

