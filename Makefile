all:
	gcc daemon.c -o daemon -lpam -lpam_misc
	gcc client.c -o client
