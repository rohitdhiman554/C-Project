#ifndef _MY_UTILITIES_
#define _MY_UTILITIES_

int isCharInString(char c, char *str);
void removeJunkCharacters(char *str);
int isArithmeticOp(char c);
int isStringEmpty(char *str);
int sign(char c);
int isUpperCase(char c);
int isLowerCase(char c);
int isAlphabet(char c);
int isDigit(char c);
int getDigit(char c);
char decodeAlphabet(int n);
int encodeAlphabet(char c);
int countAlphabets(char *line);

#endif
