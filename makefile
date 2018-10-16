%.o: %.c %.h
	gcc $< -c -g

a.out: commands.o parsing.o redirection.o piping.o shell.c
	gcc $^ -o $@ -g
