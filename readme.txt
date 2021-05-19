REMEMBER, SIGKILL CANT BE CATCHED.

TODO: might need to add some queue in case multiple signals are going to come
TODO: travelRequest: add case dateV after dateT
TODO: put a flag in funcs for parent etc



//functions for reading and writing into named pipes
//there is a specific function for writing/reading an int for simplicity
//and a specific function for writing/reading some data
//for this implementation reading and writing comes in a form of string and first is nessasary to read/write the size of the string


First of all, I've used parts of my first project in this project. These parts are in a folder called fromProjectOne.
Because of the fact that I've used them i had to do some small adjustments and add some functions to make things work
diffrently.

For bloomFilter.cpp/hpp:
I've added 2 functions, first one is a insert function that takes a string as parameter and for every index of that
string(and the bloom filter's array) im doing a logical or to append data.
Second function is toString, used to "encode" bloomFilter's data into a string so its easier for me to transport the 
bloom filter through a specific named fifo's buffer.

For classes.cpp/hpp:
Here I've created a new list called SRListHeader, used to be possible for me to return multiple vaccinations of one citizen.
Later there are cases that we need to return all the vaccinations one citizen has done(or not, we just need to have a record
with the citizen's ID) and return that and print.

For generalList.cpp/hpp and virusesList.cpp/hpp:
Simply copied the vaccineStatus functions we some modifications, added a boolean parameter to indicate that we want to use
the function that wont print any data, instead it will insert into the SRListHeader and finally return it. This modifications
are done to both vaccineStatus functions.

Now for the new parts, 
First of all for simplicity reasons I've created some files called funcs.cpp/hpp that have some functions. These functions
are used for both childs and parent of the project.
Here we have the functions for reading and writing data that travel through a named pipe. These functions are for breaking
the data into parts of bufferSize.
For both reading and writing we have 2 functions. The first function is a read/writePipeInt function that sends/receives
integers. Had to make it a seperate function because of the fact that i needed to make some changes in case the number
had less that 4 digits. Since the way that im transfering bytes through a named pipe is strings, i need to convert everything into
strings and because of the fact that a number could have <4 digits the receiver couldnt know how many bytes it need to read so,
while writing, im counting how many digits there are in this int, and if there are <4 digits im appending as many 0s needed in
the front of the number so it wouldnt matter while receiving the data and using the stoi() function to convert the string to int.

Now for writing(both strings and ints), there are two cases, first case is that the size of the buffer is smaller thant the size of the 
data we need to send so we have to break this data into smaller parts. This is easy to do simply with an index counter and a while loop
and each time send bufferSize bytes. The other case is when the size of the buffer is bigger or equal with the size of the data we need
to send, this is the simpler case since the only thing we need to do is call the read() function one time.

For reading(both strings and ints), as a rule, before each write string im writing an int with the size of the string im going to send next, 
so the receiver will know how many bytes will need to read. So for reading an int im reading 4 bytes in total(since sizeof(int) is 4), each
time im calling the read function im appending the data just read into a string variable. For when reading a string, while calling the readPipe
function i provide a int parameter called size. This function is the size of the data we are about to read so the receiver will know when to stop.
This was the case were we dont have enough bufferSize to transfere the data in one pass, for that case is as simple as one read() call.
Finally we return the complete string just read and stoi() the string if we are in the readPipeInt function.

For the files travelStatsList.cpp/hpp:
Here we have a simple list that we use to append data each time we have a new travelRequest in place.
Each node has 3 data members, first is outcome, we store if the request was accepted or rejected, second we have country, string variable used to store the
countryFrom that the request has come and finally we have date, the desired travel date that the citizen requested tor travel.
Also in the header of this list we have 3 counters used to count how many request we have, how many were accepted and how many were rejected.
Finally, besided the simple insert function we have 2 getStats functions that are used for the command /travelStats and count how many requests were 
accepted and how many were rejected in a range of two dates. The second getStats function is the same as above but we have a country in the arguments and so,
to include a record into our final statistics we are making sure that the request happen for the specific country.
More over its good to mention that in both getStats functions we first checkAndFormatDate the date to make it into yyyy-mm-dd format to be easier to compare(as did
in project 1).

For the files sList.cpp/hpp:
Here we have another simple linked list with a header, but here we have a list that its nodes are sorted strings.
This is usefull in the parent process while sorting the countries and then finding out which monitor will get each country.
This list sorts its nodes when inserting new data and its sorting it ascendingly(TODO: MAKE SURE THIS IS CORRECT) so with the pop() function we get the greater country
in the list.

For the parent proccess:
First of all we have 2 files. 
The first file is named parentCommands.cpp/hpp:
Here I've organized all the commands and SIGNAL functions that had to make it in a seperate function.
--First of all we have the function called travelRequest, used when the user sends the /travelRequest command.
Here we first(as instructed) we check if the virusName's bloom filter knows if the citizen is maybe vaccinated or not. If not we simply print the message and append the 
request record to the statistics list(TSHeader stats parameter). If the bloom filter returns something we are maybe sure that the citizen has been vaccinated so we find
the monitor that handles the countryFrom country and send a 101 protocol message indicating that we are running a travelRequest command. After that we send all the nessasary
data through the named pipe and wait for a int response indicating if the citizen is vaccinated or not. If yes, the next thing we need to do is wait for the date vaccinated, then
with a simple equation we count how many months the date of vaccination is from the traveling date. If there are less or equal than 6 months between the two dates we simply print the message,
send a 1 int to the monitor indicating that the travelRequest was accepted and finally insert the request record to the stats list.
If the monitor returned 0 or the dates are more than 6 months far we print the desired message and send/insert the correct data to the monitor/stats list.
Finally its good to mention that incase something wrong has happen(country from not inside our input file).

--For the second function, searchVaccinationStatus, we use it for the command /searchVaccinationStatus.
First of all we send to all the monitors the 104 protocol message untill we have a monitor that responds possitive meaning that the monitor found the citizen we are looking for.
If a monitor send a 1 monitor sends all the information can find for the specific citizen, sends a len with how many vaccinations the citizen has done and finally send for each vaccination 
the viruse name, an int if the citizen is vaccinated or not and if vaccinated send the date vaccinated and print all the data.
Since our citizens are unique and we can have the same citizen inside 2 monitors we return to the main function after this loop is done.

--For the third and final function, generateLogFileParent used when the program is about to finish or receive SIGINT or SIGQUIT signals.
First the function created a log_file.parentPid and print all the countries and the full statistics we have.
