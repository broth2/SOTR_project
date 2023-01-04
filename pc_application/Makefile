CC =  gcc # Set the compiler
L_FLAGS = -lrt -lpthread -lm
#C_FLAGS = -g

all: imageProcAlg cab
.PHONY: all

# Project compilation
cab: cab.c
	$(CC) $< -o $@ $(C_FLAGS) $(L_FLAGS)

imageProcAlg: imageProcAlg.c
	$(CC) $< -o $@ $(C_FLAGS) $(L_FLAGS)

	
.PHONY: clean 

clean:
	rm -f *.c~ 
	rm -f *.o
	rm imageProcAlg
	rm cab
