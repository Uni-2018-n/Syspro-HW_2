FLAGS = -std=c++11 -Wall -g -o
SOURCE = travelMain.cpp Structures/sList.cpp funcs.cpp Structures/fromProjectOne/Structures/virusesList.cpp Structures/fromProjectOne/Structures/skipList.cpp Structures/fromProjectOne/Structures/bloomFilter.cpp Structures/fromProjectOne/classes.cpp parentCommands.cpp Structures/travelStatsList.cpp
ITEM = travelMonitor
SOURCE2 = monitorMain.cpp funcs.cpp Structures/fromProjectOne/Structures/skipList.cpp Structures/fromProjectOne/Structures/bloomFilter.cpp Structures/fromProjectOne/classes.cpp Structures/fromProjectOne/generalList.cpp Structures/fromProjectOne/Structures/countryList.cpp Structures/fromProjectOne/Structures/virusesList.cpp commands.cpp
ITEM2 = Monitor

compile : $(SOURCE)
	rm -rf /tmp/fifo*
	rm -rf log_file.*
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