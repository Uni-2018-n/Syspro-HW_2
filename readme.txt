REMEMBER, SIGKILL CANT BE CATCHED.






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

For travelMain.cpp/hpp:
First of all we have a handler function that is assinged into the signaction struct, this function handles SIGINT, SIGQUIT and SIGCHLD signals and these are set at the begining of the main function.
More over its good to mention that here we have a global variable named action, this variable is being set on the signal handler. If this function is set to 1 it means that the process received a signal SIGINT or SIGQUIT,
finally this variable is getting reseted when we are doing all the nessasary actions we need because we received the signal.
In the main function, first we set the signal handler and the sigaction for each signal we need to handle. After that, a simple argument checking and argument to variable seting and opening the input directory provided
from the user. Next thing to do is inserting the correct subdirectories into a SLHeader list(skipping the . and .. directories). Because of that now we have a sorted list with all the countries located
into the provided input_dir so its time to see which monitor will get each country.
Before that though we need to check how many monitors we actually need in the program. Since there is a chance we have more monitors than countries we will only need as many monitors as countries so we wont have any monitors 
without any data. Finally after we figure that out we need to intialize and fill the 2D array storing which country each monitor will get. Since we need to give the countries alphabetically we always pop the firs(each time)
country from the list and we set it to the index of the array, (countryList.count/activeMonitors)+1 times for each monitor. 
After all of these is done its time to create the fifos for each monitor. One for writing and one for reading for each monitor. The fifos are stored at the /tmp/ directory and set with permissions 0666.
In this implementation im creating each fifo, then start the specific child with fork and finally in the child's case im using a execlp to run the ./monitor process. After that im storing the child's pid into a array to be
able to use it later. Finally for the initialization to be done im opening the named fifos for both reading and writing and im storing their file descriptors into arrays for later use.
Since sending more data than the named fifos into the arguments of the child's process isnt allowed, im sending the first data stream to the child as a sizeof(int) message to let it know how much the allowed bufferSize is.(This
happens even if the buffer is less than sizeof(int)).
After that the parent sends all the information the child will need such as, bloomFilter's array size, how many countries the child will have assinged and what countries are those.
After all of this is done, the parent waits each monitor to get ready the bloom filters so the parent will receive them and store them. Here we have a simple sent how many viruses we have first, then for each virus send
the size of the string that the bloom filter is encrypted to(and send a verification message) and finally receive the bloom filter.
Each read-write that the processes are doing are happening with the functions in the funcs.cpp/hpp files.
After that is done the parent proccess receive a confirmation message indicating that the monitor is ready to receive commands and signals.
For the parent though there is one more thing to be done before being ready for commands, we need to extract the name of the virus from the encoded string and append the bloom filter just read into the existing(or create a new one)
bloom filter that the parent might have.
Finally the parent is ready for commands and letting the user know its ready by sending all the available commands and printing the waiting for command message.

For signal action handling and command handling we have a while loop with a switch case inside. The default case is the case were the program waits the user to send commands, this teqnique is the same used in the previous project with
changes at the /exit command, the functions called for each command and the command text the user will write. Also one major change is that at the end of each while loop we are checking if the action variable changed while we were on a command
so we break from the inner while loop and we go through the switch-cases to handle the specific signal.
Now for the signal action handling as we've said before, if the action is se to 1 we are handling SIGINT and SIGQUIT signals, so for that case, since we want to stop the program we do as the /exit user command does, first we delete any allocated memory
then for each monitor we are sending a SIGKILL signal to kill them and wait for the child process to stop so we wont have any zombie processes. After that for each monitor we close and unlink the file descriptors and the fifos so they will get removed.
Finally as the assignment says, we generate a logfile with all the nessasary data needed(as said before) and delete the remaining allocated memory.
For case the action variable is set to 2 indicating we had a SIGCHLD signal, we wait for the child to receive the exit code and find the index of the child in the monitorPids array. After that we need to close and unlink the previous named fifo and create
a fresh named fifo and child process the same as the begining of the travelMonitor's main function so the new monitor will have the same data as the old one had.

