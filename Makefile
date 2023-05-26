kilo: kilo.c   #what we want to build : what required to build it
		$(CC) kilo.c -o kilo -Wall -Wextra -pedantic -std=c99

# -Wall stans for "all Warnings"
# -Wextra and -pedantic turn on more warnings
# -std=c99 specifies the exact version of the C language that you are using

