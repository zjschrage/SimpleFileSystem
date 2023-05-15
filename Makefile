CC = gcc -g -Wall #-fsanitize=address,undefined
G_INC = -Iinclude $(CPPFLAGS)
O = -o fs
DEPS = *.c

all: clean fs

fs:
	$(CC) $(G_INC) $(O) $(DEPS)

clean:
	$(RM) fs