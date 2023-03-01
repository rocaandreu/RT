#ifndef PARAMS_H
#define PARAMS_H

int str_size = 20;
int EQUAL = 0;
int NOT_EQUAL = 2;
int SHORTER_THAN_SR2 = -1;
int LONGER_THAN_SR2 = 1;
int INVALID_NUMBER = -1;

int StringLength(char *sr);
int StringCompare(char *sr1, char *sr2);
int AsciiToInteger(char *sr1);
char HexDigit(int dec);
char *DecToHex(int dec);


#endif