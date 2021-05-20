FLAGS = -std=c++11 -Wall -g -o
SOURCE = travelMain.cpp sList.cpp funcs.cpp fromProjectOne/Structures/virusesList.cpp fromProjectOne/Structures/skipList.cpp fromProjectOne/Structures/bloomFilter.cpp fromProjectOne/classes.cpp parentCommands.cpp travelStatsList.cpp
ITEM = travelMonitor
SOURCE2 = monitors/monitorMain.cpp funcs.cpp fromProjectOne/Structures/skipList.cpp fromProjectOne/Structures/bloomFilter.cpp fromProjectOne/classes.cpp fromProjectOne/generalList.cpp fromProjectOne/Structures/countryList.cpp fromProjectOne/Structures/virusesList.cpp monitors/commands.cpp
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