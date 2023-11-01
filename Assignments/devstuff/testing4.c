#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUFFER_SIZE 52

int main(int argc, char* argv[]) {
	char **dictList = (char**)malloc(0 * sizeof(char*));

	int dicSize = 0;

	FILE* dicFile = fopen("/usr/share/dict/words", "r");

	char s[MAX_BUFFER_SIZE];
	/*for (int i = 0; i < 256; i++) {
		dictList = realloc(dictList, ++dicSize * sizeof(char*));
		dictList[i] = malloc(MAX_BUFFER_SIZE);
		fgets(dictList[i], MAX_BUFFER_SIZE, dicFile);
	}*/
	
	printf("sizeof argv[1]: %d\n", strlen(argv[1]));
	
	while (fgets(s, MAX_BUFFER_SIZE, dicFile) != NULL) {
		strtok(s, "\n");
		printf("%s\n", s);
		dicSize++;
		dictList = realloc(dictList, dicSize * sizeof(char*));
		dictList[dicSize - 1] = (char*)malloc(MAX_BUFFER_SIZE);
		strcpy(dictList[dicSize - 1], s);
	}
	
	fclose(dicFile);
	
	for (int i = 0; i < 256; i++) {
		free(dictList[i]);
	}
	
	free(dictList);

	return 0;
}
