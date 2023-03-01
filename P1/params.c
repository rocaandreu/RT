#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "params.h"


int main(int argc, char *argv[]){

   //params action param1 param2
   if (argc < 3)
   {
      write(1, "ERROR: Sintaxis incorrecta\nSintaxis esperada: ./params.out action param1 [param2]\n", 83);
      return -1;
   }

   if (StringCompare(argv[1], "compare") == EQUAL)
   {
      if (argc < 4)
      {
         write(1, "ERROR: Número de parámetros incorrecto para la acción \"compare\"\n", 68);
         write(1, "Sintaxis esperada: ./params.out action param1 [param2]\n", 56);
         return -1;
      }
      
      int compare = StringCompare(argv[2], argv[3]);
      switch (compare)
      {
      case 0:
         write(1,"Las dos palabras son iguales\n", 30);
         break;
      case 1:
         write(1,"La primera palabra es más larga\n", 34);
         break;
      case -1:
         write(1,"La segunda palabra es más larga\n", 34);
         break;
      default:
         write(1,"Las palabras no son iguales\n", 29);
         break;
      }      
   }
   else if (StringCompare(argv[1], "number") == EQUAL)
   {
      char *hex;
      hex = DecToHex(AsciiToInteger(argv[2]));
      write(1, hex, StringLength(hex));
      write(1, "\n", 1);
      free(hex);
   }
   else
   {
      int charCount = 0; 
      for (int i = 0; i < StringLength(argv[2]); i++)
      {
         if (argv[2][i] == argv[1][0] || argv[2][i]+32 == argv[1][0] || argv[2][i] == argv[1][0] + 32)
         {
            charCount++;
         }
      }
      char buf[44];  
      int len = snprintf(buf, 32, "El carácter %c aparece %d veces\n", argv[1][0], charCount);
      write(1, buf, 44);
   }
   return 0;
}


int StringLength(char *sr){
   int i = 0;
   while (sr[i] != '\0')
   {
      i++;
   }
   return i;
}


int StringCompare(char *sr1, char *sr2){
   int len1 = StringLength(sr1), len2 = StringLength(sr2), i = 0;
   
   if (len1 > len2)
   {
      return LONGER_THAN_SR2;
   }
   else if (len1 < len2)
   {
      return SHORTER_THAN_SR2;
   }
   else
   {
      while(sr1[i] != '\0' && sr2[i] != '\0')
      {
         //+-32 to make the comparative non case-sensitive
         if (sr1[i] != sr2[i] && sr1[i] != sr2[i]+32 && sr1[i]+32 != sr2[i])
            return NOT_EQUAL;
         i++;
      }
      return EQUAL;
   }
}
   

int AsciiToInteger(char *sr1){
   int i, res = 0, digit = 1, length = StringLength(sr1);
   for (i = length-1; i >= 0; i--)
   {
      if (sr1[i] < 48 || sr1[i] > 57)
         return INVALID_NUMBER;

      res += (sr1[i]-48)*digit;
      digit *= 10;
   }
   return res;
}

//Converts a number [0-15] to a single HEX digit
char HexDigit(int dec){
   if (dec < 10)
      return dec + 48;
   else if (dec < 16)
      return dec + 55;
   else
      return 'X';
}


char *DecToHex(int dec){
   int module, i = 0, len;
   char *aux = malloc (sizeof (char) * 20);
   char *hex = malloc (sizeof (char) * 20);

   //Do the conversion and store the characters in an aux string
   while (dec >= 16)
   {
      module = dec % 16;
      dec /= 16;
      aux[i] = HexDigit(module);
      i++;
   }
   aux[i] = HexDigit(dec);
   
   //Reverse the resulting string and return
   len = StringLength(aux);

   for (i = 0; i < len; i++)
   {
      hex[i] = aux[len-i-1];
   }
   
   free(aux);   
   return hex;
}