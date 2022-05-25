/*
* Connect Four, Part3 (Server) by Joshua Cho (001421649) and Alexender Meltzer-Werner (001262233)
* Instruction (Server): ./compiledFileName PortNumber (or) ./compiledFileName    *port number is optional. If you don't provide it, this program will generate it for you.
* Instruction (Client): ./compiledFileName IPaddressOfServer PortNumber
* Please do not type a column on your command window unless you see "Please enter a column from a-g: "
*/

#include <stdio.h> 
#include <string.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>

int exitvalue = 0; //If this value is 9999, then the program will end. 
int socket_bt_ser_cli; //socket between server and client
int socketType; //socketType
char message[10]; //this will store column

/*
* This method allocates memory for this game. (2 dimensional array)
*/
void initialization(char* nuser2, char*** graph, int* socket_bt_ser_cli) {

	printf("Setting up the game...\n\n");

	*graph = malloc(sizeof(char*) * 6); // 4*6 size byte

	for (int i = 0; i < 6; i++) {
		(*graph)[i] = malloc(sizeof(char*) * 7); //4*7 byte
	}

	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			(*graph)[i][j] = '_';

		}
	}
}

/*
* This method prints "Destroying the game", frees memory, sets the exitvalue to 9999, and closes sockets.
*/
void teardown(char*** graph) {

	printf("\n");
	printf("Destroying the game...\nGame Over!\n\n\n");
	exitvalue = 9999;
	free(*graph);
	close(socketType);
	close(socket_bt_ser_cli);
}

/*
* This method is called when one of the users creates connect4 and will call teardown() function.
*/
void win(char*** graph, int value, char wincolumn) {

	printf("\n");
	if (value == 2) {
		printf("You(Server) won!\n");
		bzero(message, 10);
		message[0] = wincolumn;
		write(socket_bt_ser_cli, message, 10);
	}
	else if (value == 1) printf("Sorry... Client won.\n");
	teardown(graph);
}

/*
* This method displays the current state of the world (board).
*/
void DisplayWorld(char*** graph) {

	printf("\n");
	printf("The World Right Now: ");
	printf("\n\nA B C D E F G\n");
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			printf("%c ", (*graph)[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

/*
* This method accepts a letter for whih column of the game we are going to drop a disc into.
* Here, a user will be able to type a letter.
*/
void AcceptInput(char* message) {
	bzero(message, 10);
	printf("Please enter a column from a - g: ");
	fflush(stdout);
	fgets(message, 10, stdin);
}

/*
* This method passes the input data to this function and will call teardown() if the user tries to input
a value to a full column and win() function if the user creates a connect4
*/
void UpdateState(char*** graph, char* value, int turnnum) {

	int column = 0;
	int row = 0;
	int flag = 0;
	char player = turnnum == 1 ? 'x' : 'o';
	if (*value == 'a' | *value == 'A') column = 0;
	if (*value == 'b' | *value == 'B') column = 1;
	if (*value == 'c' | *value == 'C') column = 2;
	if (*value == 'd' | *value == 'D') column = 3;
	if (*value == 'e' | *value == 'E') column = 4;
	if (*value == 'f' | *value == 'F') column = 5;
	if (*value == 'g' | *value == 'G') column = 6;

	if (((*graph)[0][column] != '_') && turnnum != 1)
	{

		char otherValue; //this will store the changed column from the user

		printf("\n\nThe column you have entered is already full.\nPress another column value a-g or press q or Q if you would like to exit: ");

		scanf("%c", &otherValue);

		getchar(); //For enter key

		printf("\n");

		if (otherValue == 'q' || otherValue == 'Q') teardown(graph);

		else {

			column = otherValue;
			*value = otherValue;

			if (column == 'a' | column == 'A') column = 0;
			if (column == 'b' | column == 'B') column = 1;
			if (column == 'c' | column == 'C') column = 2;
			if (column == 'd' | column == 'D') column = 3;
			if (column == 'e' | column == 'E') column = 4;
			if (column == 'f' | column == 'F') column = 5;
			if (column == 'g' | column == 'G') column = 6;
		}
	}
	
	if (exitvalue != 9999)
	{
		while ((row < 6) && (*graph)[row][column] == '_')
			++row;
		row -= 1;
		(*graph)[row][column] = player;

		//verrtical check
		if (row + 3 < 6)
		{
			flag = (((*graph)[row][column] == player) && ((*graph)[row + 1][column] == player) &&
				((*graph)[row + 2][column] == player) && ((*graph)[row + 3][column] == player));
		}
		//horizontal check
		if (flag == 0)
		{
			for (int i = 0; i < (7 - 3) && (flag == 0); ++i)
			{
				flag = (((*graph)[row][i] == player) && ((*graph)[row][i + 1] == player) &&
					((*graph)[row][i + 2] == player) && ((*graph)[row][i + 3] == player));
			}
		}
		//starting check posistion down/right
		if (flag == 0)
		{
			int r, c;
			if (column - row < 0)
			{
				r = row - column;
				c = 0;
			}
			else
			{
				r = 0;
				c = column - row;
			}
			//diagonal check down/right
			while ((r < 6 - 3) && (c < 7 - 3) && (flag == 0))
			{
				flag = (((*graph)[r][c] == player) && ((*graph)[r + 1][c + 1] == player) &&
					((*graph)[r + 2][c + 2] == player) && ((*graph)[r + 3][c + 3] == player));
				r++;
				c++;
			}
			//starting check posistion down/left
			if (column + row < 6)
			{
				r = 0;
				c = row + column;
			}
			else
			{
				r = (row + column) - 6;
				c = 6;
			}
			//diagonal check down/left
			while ((r < 6 - 3) && (c >= 7 - 4) && (flag == 0))
			{
				flag = (((*graph)[r][c] == player) && ((*graph)[r + 1][c - 1] == player) && ((*graph)[r + 2][c - 2] == player) &&
					((*graph)[r + 3][c - 3] == player));
				r++;
				c--;
			}
		}
		else if ((*graph)[0][column] != '_')
		{
			printf("\nWe will end this game as the column you entered is full");
			teardown(graph);
		}
		if (flag)
		{
			char wincolumn = column + 97; //converts the numerical value of column in to char value (alphabet)
			if (turnnum == 1)
			{
				DisplayWorld(graph);
				win(graph, 1, 'z');
			}
			else
			{
				DisplayWorld(graph);
				win(graph, 2, wincolumn);
			}
		}
	}
}

/*
* Called when there's a standard error during a call to a system or library function.
*/
void error_handle(char* errorMessage)
{
	perror(errorMessage);
	exit(1); //indication of abnormal termination of a program
}

int main(int argc, char* argv[]) {

	char user2[100]; //not used but keeping it just in case
	char display[120]; //not used but keeping it just in case
	char turn[20]; //not used but keeping it just in case
	int turnnum = 0; //the value indicating whose turn it is
	char columnalpha[7] = { 'A','B','C','D','E','F','G' }; //the column names
	char value = 0; //not used but keeping it just in case
	int i = 0; //not used but keeping it just in case

	char** graph; //pointer for 2 dimensional array

	struct sockaddr_in server_address, client_address; //server address and client address
	int portnum; //port number

	int cli_add_size; //just needed for accept() parameter. This is the size of the client address size.

	int error_check; //This will be a value that will checks whether read() and write() occurred properly.


	char hostbuffer[100]; //host name
	struct hostent* host_entry;
	char* IPbuffer; //IP address
	int hostname;
	hostname = gethostname(hostbuffer, sizeof(hostbuffer));

	host_entry = gethostbyname(hostbuffer);
	if (host_entry == NULL) {
		error_handle("Error occurred at gethostbyname()");
	}
	IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

	//This is when the user does not put the port number and this program will automatically generate available port number. 
	if (argc < 2) {
		portnum = 2000; //This number will change is 2000 is used by other process trough while() loop below. 
	}
	else portnum = atoi(argv[1]); //This is when the user puts the port number

	socketType = socket(AF_INET, SOCK_STREAM, 0);
	if (socketType < 0) error_handle("Error at socket()");

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(portnum);

	//This allows us to find the available port number from 2000 to 65535.
	while ((bind(socketType, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) && portnum < 65535) {
		portnum++;
		server_address.sin_port = htons(portnum);
	}

	printf("\nAnother user with your IP address and port number will connect to you soon. Please wait.\n");
	printf("\nPort: %d\n\n", portnum);

	fflush(stdout); //without this, printf() won't be shown until server to client communication process is over

	listen(socketType, 5); //5 is the maximum amount of client

	cli_add_size = sizeof(client_address); //Just needed as a paramter for accept(). This is client address size
	socket_bt_ser_cli = accept(socketType, (struct sockaddr*)&client_address, &cli_add_size); //officially created socket between server and client
	if (socket_bt_ser_cli < 0) error_handle("Error while connecting with the other player. Please try again.\n");


	initialization(user2, &graph, &socket_bt_ser_cli); //Calling initialization() function to set up the game.

	printf("Direction:\n");
	printf("Whoever first creates connect 4 is the winner!\n");
	printf("In addition, if the column the player chose is full, you will have an option to either quit the game or choose another column.\n\n");

	printf("Client will go first and will be x. Server will go after and will be o\n");

	DisplayWorld(&graph);

	for (; ; ) { //infinite for loop

		bzero(message, 10); //clear message
		read(socket_bt_ser_cli, message, 10); //message is the column value
		if (message[0] == '\0') {
			printf("Client has quit the game. Good Bye!\n\n");
			return 0;
		}
		printf("\nClient chose column %c\n", message[0]);
		char value = message[0];
		UpdateState(&graph, &value, 1); //calling updateState function to update the board
		if (exitvalue == 9999) return 0; //if exitvalue is 9999, end the program
		DisplayWorld(&graph); //calling DisplayWorld function to display the current state of the board

		AcceptInput(message); //call AcceptInput() to type a column value.
		UpdateState(&graph, message, 2);//calling updateStaet function to update the board
		if (exitvalue == 9999) return 0; //if exitvalue is 9999, end the program
		write(socket_bt_ser_cli, message, 10);
		DisplayWorld(&graph);//calling DisplayWorld function by passing the current state of the board

	}
}


