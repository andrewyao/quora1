default:
	clear
	@gcc -std=c99 -O3 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Werror ac.c -o ac
	@date
	@cat 78.txt|./ac
	@date

profile:
	clear
	@gcc -std=c99 -O3 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Werror ac.c -o ac -pg
	@date
	@cat 78.txt|./ac
	@date

memcheck:
	clear
	@gcc -std=c99 -O3 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual -Werror ac.c -o ac
	cat 76.txt |valgrind -v --leak-check=full --tool=memcheck ./ac 2> output
