#include "base64.h"

#include <stdlib.h>
#include <string.h>

int valof(char c) {
        static char * alphabet =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char * tmp = alphabet;
	int i = 0;

	if (c == '\0')
		return 0;
	
	while(1) {
		if (*tmp == '\0')
			return 0;
		if (*tmp == c)
			return i;
		i++;
		tmp++;
	}
}

void nextchunk(char ** c, char * chunk) {
	int i;
	int b64val[4];

	for(i=0; i < 4; i++) {
		b64val[i] = valof(**c);
		if(**c != '\0')
			(*c)++;
	}
	
	*(chunk + 0) = CHAR1(b64val[0],b64val[1]);
	*(chunk + 1) = CHAR2(b64val[1],b64val[2]);
	*(chunk + 2) = CHAR3(b64val[2],b64val[3]);
	
}

char * b64dec(char * msg) {
	char * tmp = msg;
	char * buffer;
	char chunk[4];

	if(*tmp == '\0')
		return NULL;
	
	memset(chunk,0,sizeof(chunk));
	buffer = (char *)malloc(strlen(msg) + 1);
	
	while(*tmp != '\0') {
		nextchunk(&tmp,&chunk[0]);
		strcat(buffer,chunk);
	}
	
	return buffer;
}


int main(void) {
	char * msg = "ZGF2aWQ6ZGF2aWQ=";
	printf("The answer is %s\n",b64dec(msg));
}
	
