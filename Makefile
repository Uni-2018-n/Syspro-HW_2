FLAGS = -std=c++11 -Wall -g -o
SOURCE = main.cpp
ITEM = travelMonitor

compile : $(SOURCE)
	# gcc $(FLAGS) $(ITEM) $(SOURCE)
	g++ $(FLAGS) $(ITEM) $(SOURCE)

run : compile
	./$(ITEM)
	
mem : compile
	valgrind --leak-check=full ./$(ITEM) $(ARGS)

debug : $(ITEM)
	gdb $(ITEM)

clean :
	-rm $(ITEM)

