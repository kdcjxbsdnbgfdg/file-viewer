#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <asm-generic/ioctls.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "arraylist.c"

#define INPUT_SIZE 64

char startsWith(char *inputString, char *startString){
	if (strlen(startString) > strlen(inputString)) return 0;
	while (*startString != 0){
		if (*startString != *inputString) return 0;
		inputString++; 
		startString++;
	}
	return 1;
}

char *getInput(){
	char *input;
	fgets(input, INPUT_SIZE, stdin);
	return input;
}


int main(int argc, char *argv[]){
	// enable alternate screen buffer
	printf("\e[?1049h");
	// hides the cursor
	printf("\e[?25l");
	// puts cursor at top left
	printf("\e[1;1H");
	//printf("Press escape to exit\n");

	struct termios attr, newAttr;
	tcgetattr(0, &attr);
	newAttr = attr;
	// turns off icanon and echo mode
	// turning off canonical mode makes input immediately available
	// turning off echo mode 
	newAttr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(0, 0, &newAttr);

	struct winsize termsize;
	ioctl(0, TIOCGWINSZ, &termsize);
	int xSize = termsize.ws_col;
	int ySize = termsize.ws_row;

	DIR *dirstream;
	struct dirent *directoryEntry;
	char *workingDirectory;
	if (argc > 1){
		workingDirectory = argv[1];
	}
	else {
		workingDirectory = "/";
	}
	dirstream = opendir(workingDirectory);
	if (dirstream == NULL){
		// todo add some kind of error handling
		goto cleanup;
	}
	stringList list = createStringList(32);
	while ((directoryEntry = readdir(dirstream)) != NULL){
		addStringToList(&list, directoryEntry->d_name);
	}
	for (int i = 0; i < list.size; i++){
		for (int j = i + 1; j < list.size; j++){
			// if string a is less than string b, then the postions should stay the same
			// if string a is more than string b, then the positions should swap
			// strcmp returns a positive value if string a is more than string b
			// ABA, ABZ, ABA should go first, ABA is less than ABZ
			if (strcmp(list.data[i], list.data[j]) > 0 ){
				char *swap = list.data[i];
				list.data[i] = list.data[j];
				list.data[j] = swap;
			}
		}
	}
	closedir(dirstream);

	for (int i = 0; i < ySize - 1 && i < list.size; i++){
		printf(" %s\n", list.data[i]);
	}

	int cursorY = 0;
	int scrollNum = 0;
	int inputSize = INPUT_SIZE;
	char *input = malloc(INPUT_SIZE);
	while (1){
		char cursorMoved = 0;
		int bytesRead = read(STDIN_FILENO, input, INPUT_SIZE);
		input[bytesRead] = 0;
		if (!strcmp(input, "")) break;
		else if (!strcmp(input, "\e[A")) {
			if (cursorY > 0) cursorY--;
			cursorMoved = 1;
		}
		else if (!strcmp(input, "\e[B")) {
			if (cursorY < list.size) {
				cursorY++;
				cursorMoved = 1;
			}
		}
		else if (!strcmp(input, "\e[D")) printf("left\n");
		else if (!strcmp(input, "\e[C")) printf("right\n");
		else printf("%s\n", input);

		if (cursorY > scrollNum + ySize){
			scrollNum++;
		}
		else if (cursorY < scrollNum){
			scrollNum--;
		}
		else {
			goto noscroll;
		}
		printf("\e[1;1H");
		for (int i = scrollNum; i < ySize + scrollNum - 1 && i < list.size; i++){
			printf("\e[K");
			printf(" %s\n", list.data[i]);
		}
noscroll:

		if (cursorMoved){
		}
	}
cleanup:
	tcsetattr(0, 0, &attr);
	// reappear cursor
	printf("\e[?25h");
	// disable alternate screen buffer
	printf("\e[?1049l");
	return 0;
}
