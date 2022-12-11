CC =  gcc # Set the compiler
L_FLAGS = -lrt -lpthread -lm
#C_FLAGS = -g

all: img_proc
.PHONY: all

# Project compilation
img_proc: imageProcAlg.c
	$(CC) $< -o $@ $(C_FLAGS) $(L_FLAGS)

	
.PHONY: clean 

clean:
	rm -f *.c~ 
	rm -f *.o
	rm img_proc
