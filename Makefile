FLAGS = -std=c++11 -Wall -g -o
SOURCE = travelMain.cpp sList.cpp
ITEM = travelMonitor
SOURCE2 = monitorMain.cpp
ITEM2 = monitor

compile : $(SOURCE)
	# gcc $(FLAGS) $(ITEM) $(SOURCE)
	g++ $(FLAGS) $(ITEM) $(SOURCE)
	g++ $(FLAGS) $(ITEM2) $(SOURCE2)

run : compile
	./$(ITEM)
	
mem : compile
	valgrind --leak-check=full ./$(ITEM) $(ARGS)

debug : $(ITEM)
	gdb $(ITEM)

clean :
	-rm $(ITEM)

