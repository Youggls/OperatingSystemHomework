PROGRAM = main
CC = gcc
DEPS = $(shell find ./ -name "*.h")
SRC = $(shell find ./ -name "*.c")
OBJ = $(SRC:%.cpp=%.o)

$(PROGRAM): $(OBJ)
	$(CC) -o $(PROGRAM) $(OBJ) -g

%.o: %.cpp $(DEPS)
	$(CC) -c $< -o $@ -g

clean:
	rm -rf *.o main
