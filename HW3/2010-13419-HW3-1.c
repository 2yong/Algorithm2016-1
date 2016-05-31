#include <stdio.h>

#define STEP 50	/* step length */
#define SECT 100	/* section width */

#define UP 0
#define LEFT 1
#define DIAG 2

#define MAX(a, b) a > b? a : b

void LCSequencing(FILE *fileA, FILE *fileB, char cA, char cB);
int myfget(char *dst, int size, FILE *input);
int lcs(char *srcA, char *srcB, int lenA, int lenB);
int lcsprint(char *srcA, char *srcB, int lenA, int lenB);

int main(int argc, char **argv){
	FILE *inA, *inB; 

	/* file opening & error checking */
	if (argc != 3) {
		printf("Error: Check the arguments. usage: cmd <file1> <file2>\n");
		return -1;
	}

	inA = fopen(argv[1], "r");
	if (inA == NULL) {
		printf("Error: Check the path of first file.\n");
		return -1;
	}	

	inB = fopen(argv[2], "r");
	if (inB == NULL) {
		printf("Error: Check the path of second file.\n");
		return -1;
	}

	/* main procedure. LCS sequencing and printing */
	LCSequencing(inA, inB, 'A', 'B');
	LCSequencing(inB, inA, 'B', 'A');

	/* file closing */
	fclose(inA);
	fclose(inB);

	return 0;
}

void LCSequencing(FILE *fileA, FILE *fileB, char cA, char cB){
	long topA, topB;
	char headA[SECT], headB[SECT], tmpA[SECT], tmpB[SECT];
	int curA, curB; 

	/* putting the header away and remember current position */
	fseek(fileA, 0, SEEK_SET);
	fseek(fileB, 0, SEEK_SET);
	fgets(headA, SECT, fileA);
	fgets(headB, SECT, fileB);
	topA = ftell(fileA);
	topB = ftell(fileB);

	curA = 0;
	while(!feof(fileA)){
		int lenA, lenB, curlen=0, maxlen=0, maxB=0;
		char sequence[SECT] = {'\0', };

		fseek(fileA, topA + (curA*STEP) + (curA/2)*2, SEEK_SET);
		lenA = myfget(tmpA, SECT, fileA);
		if(lenA <= STEP) break;

		curB = 0;
		while(!feof(fileB)){
			fseek(fileB, topB + (curB*STEP) + (curB/2)*2, SEEK_SET);
			lenB = myfget(tmpB, SECT, fileB);
			if(lenB <= STEP) break;

			curlen = lcs(tmpA, tmpB, lenA, lenB); 
			if (maxlen < curlen) {
				maxlen = curlen;
				maxB = curB;
			}
			curB++;
		}
		
		fseek(fileB, topB + (maxB*STEP) + (maxB/2)*2, SEEK_SET);
		lenB = myfget(tmpB, SECT, fileB);

		printf("%c%d,%c%d\n", cA, curA*STEP, cB, maxB*STEP);
		lcsprint(tmpA, tmpB, lenA, lenB);

		curA++;
	}
}

/* ignoring '\n' and '\r', put 'size' characters into 'dst' from 'input.
 * return total characters which have been read.
 */
int myfget(char *dst, int size, FILE *input){
	int c, cnt = 0;
	char *tmpptr = dst;

	while (cnt < size && (c = fgetc(input)) != EOF){
		if (c == '\r' || c == '\n') continue;
		*tmpptr++ = c;
		cnt++;
	}

	*tmpptr = '\0';
	return cnt;
}

int lcs(char *srcA, char *srcB, int lenA, int lenB){
	char length[SECT+1][SECT+1];
	int i, j;

	for(i=0; i<SECT+1; i++){
		length[0][i]=0;
		length[i][0]=0;
	}

	srcA--;
	srcB--;

	for(i=1; i<lenA+1; i++){
		for(j=1; j<lenB+1; j++){
			if (srcA[i] == srcB[j]){
				length[i][j] = length[i-1][j-1] + 1;
			} else {
				length[i][j] = MAX(length[i-1][j], length[i][j-1]);
			}
		}
	}

	return length[lenA][lenB];
}

int lcsprint(char *srcA, char *srcB, int lenA, int lenB){
	char length[SECT+1][SECT+1], making[SECT+1][SECT+1];
	int i, j;
	char resA[SECT], resB[SECT];

	for(i=0; i<SECT+1; i++){
		length[0][i]=0;
		length[i][0]=0;
	}

	srcA--;
	srcB--;

	for(i=1; i<lenA+1; i++){
		for(j=1; j<lenB+1; j++){
			if (srcA[i] == srcB[j]){
				length[i][j] = length[i-1][j-1] + 1;
				making[i][j] = DIAG;
			} else if(length[i-1][j] > length[i][j-1]) {
				length[i][j] = length[i-1][j];
				making[i][j] = UP;
			} else {
				length[i][j] = length[i][j-1];
				making[i][j] = LEFT;
			}
		}
	}

	i--;
	j--;
	
	resA[i] = '\0';
	resB[j] = '\0';

	while(length[i][j]>0) {
		switch (making[i][j]) {
		case DIAG:
			resA[i-1] = srcA[i];
			resB[j-1] = srcB[j];
			i--;
			j--;
			break;
		case UP:
			resA[i-1]='_';
			i--;
			break;
		case LEFT:
			resB[j-1]='_';
			j--;
			break;
		default:
			printf("%d", making[i][j]);
		}
	}

	while(i-->0) resA[i]='_';
	while(j-->0) resB[j]='_';

	printf("%s\n%s\n", resA, resB);

	return length[lenA][lenB];
}
