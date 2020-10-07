#include "neillncurses.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

/* ! ~ ~ ~ ~ ~ ~ ~ ~ ~ ~  WIREWORLD ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ! */
/*                 Andrew Thomas 25/10/2019               */

typedef enum { False, True } Bool;

/* Size of input grid 40x40 expected from txt file */
#define SIZE 40

/*  Animation delay in 1/1000s of a second */
#define DELAY 100.00

/* Intended number of input strings from user (inclusive of
programme name) */
#define FILECOUNT 2

/* Minimum file size in bytes which can contain a 40x40
grid of characters. */
#define MIN_FILE_SIZE 1600

void testing(void);

/* File checking functions */
int filecheck(int argc);

int filechecksize(int size);

/* Character checking functions */ 
bool validcharcheck(int i);

bool coppercheck(int i);

bool headcheck(int i);

bool getfile(FILE *ifp, int argc);

/* Function amends input char 'H' to 't' and 't' to 'c' 
or if not H or t, returns null */
char charflip(char c);

/* Prevents accessing of memory outside of defined array */
bool boundarycheck(int a, int b);

void copyarray(char array[SIZE][SIZE], char temparray[SIZE][SIZE]);

/* Chars are amended in new array via this func */
void arrayamend(char array[SIZE][SIZE]);

void invalidmsg(int invalidflag);

void colourscheme(NCURS_Simplewin *sw);

void populatearray(char a[SIZE][SIZE], FILE *ifp, int invalidflag);

int main(int argc, char **argv)
{

   char a[SIZE][SIZE];
   long int filesize;
   int invalidflag = 0;

   FILE *ifp = NULL; 
   NCURS_Simplewin sw;

   testing();
  
   /* File opening and checking functions */
   ifp = fopen(argv[1], "r");
   if(getfile(ifp, argc) == False){ exit(1); }
   fseek(ifp, 0L, SEEK_END);
   filesize = ftell(ifp);
   rewind(ifp);
   filechecksize(filesize);  

   /* Populates 2d array with characters from file */
   /* Removes any invalid chars and replaces them with blanks */
   populatearray(a, ifp, invalidflag);
   invalidmsg(invalidflag);
   fclose(ifp);

   Neill_NCURS_Init(&sw);
   colourscheme(&sw);

   do{
      Neill_NCURS_PrintArray(&a[0][0], SIZE, SIZE, &sw);
      arrayamend(a);
      Neill_NCURS_Delay(DELAY);
      Neill_NCURS_Events(&sw);
     
      }while(!sw.finished);

   atexit(Neill_NCURS_Done);
   exit(EXIT_SUCCESS);

   }

/* ¬¬ FUNCTIONS BELOW THIS POINT ¬¬ */

/* Populates a 2D array with contents from the file */
void populatearray(char a[SIZE][SIZE], FILE *ifp, int invalidflag){

   char c; int rows, columns;

   for(rows = 0; rows < SIZE; rows++){
      for(columns = 0; columns < SIZE; columns++){
         if(((c = getc(ifp)) != '\n') || ((c = getc(ifp)) != '\0')){
            a[rows][columns] = c;
            } 

         if(validcharcheck(c) == False && c != '\0' && c != '\n'){
            a[rows][columns] = ' ';
            invalidflag++;
            }
         }
      }
}


bool getfile(FILE *ifp, int argc){

   if(filecheck(argc)){
      return True;
      }

   if(ifp == NULL){
      fprintf(stderr, "Failed to open a file. \n");
      return False;
      }

   return False;
}

/* Prevents user inputting more than one filename */
int filecheck(int argc){

   if (argc > FILECOUNT){
      fprintf(stderr, "Error: %d filenames have been input. " 
      "Please input only one filename. \n", argc-1);
      exit(1);
      return 0;
      }

   if (argc == FILECOUNT){
      return 1;
      }

   return 0;
   }

/* Prevents accessing of file which is too small (< 1600 bytes) */
int filechecksize(int size){

   if(size < MIN_FILE_SIZE){
      fprintf(stderr, "File is too small. Must contain a 40x40" 
      " grid of characters. \n");
      exit(1);
      return 0;
      }

   else{
      return 1;
      }

   }

bool validcharcheck(int i){

   return (i == 't' || i == 'c' || i == 'H' || 
           i == ' ');

   }

bool headcheck(int i){

   return (i == 'H');

   }

bool coppercheck(int i){

   return (i == 'c');

   }

/* Amends t char to c, H to t */
char charflip(char c){

   if(c == 't'){
      return 'c';
      }

   if(c == 'H'){
      return 't';
      }

   return 0;

   }

/* Function to check we are not accessing data outside 
of array boundaries. Returns true if within boundary. */
bool boundarycheck(int length, int width){

   return(width >= 0 && length >= 0 && width < SIZE 
   && length < SIZE) ? True : False;

   }

/* Helper function - just copies array */
void copyarray(char array[SIZE][SIZE], char temparray[SIZE][SIZE]){

   int rows, columns;

   for(rows = 0; rows < SIZE; rows++){
      for(columns = 0; columns < SIZE; columns++){
         temparray[rows][columns] = array[rows][columns];
         }

      }

   }

void arrayamend(char array[SIZE][SIZE]){

   int rows, columns, m, p, headcount;
   char temparray[SIZE][SIZE];

   /* Populates identical temparray with last iteration */
   copyarray(array, temparray);

    /* The relevant characters are amended within these 
    two loops */
   for(rows = 0; rows < SIZE; rows++){
      for(columns = 0; columns < SIZE; columns++){
         headcount = 0;

         /* Any t and h chars are replaced with t and h chars */
         if(charflip(temparray[rows][columns])){
            array[rows][columns] = 
            charflip(temparray[rows][columns]);
            };

         /* Checks if cell is copper and then whether surrounding 
         cells contain heads */
         if(coppercheck(temparray[rows][columns])){
            for(p = -1; p < 2; p++){
               for(m = -1; m < 2; m++){
                  if(headcheck(temparray[rows+p][columns+m]) && 
                     boundarycheck(rows+p,columns+m)){
                     headcount++;
                     }
                  }
               }

            if(headcount == 1 || headcount == 2) {
               array[rows][columns] = 'H';
               }       

            }

         }

      }

   }

void invalidmsg(int invalidflag){

   if(invalidflag > 0){
      fprintf(stderr, "Please note: There are invalid characters (%d) in "
      "the text file which have been replaced with blank space. \n", 
      invalidflag);
      }
   }

void colourscheme(NCURS_Simplewin *sw){

   /* Colourscheme for Ncurses: 'c's are set to yellow, 
   'H's are set to blue and 't's to red. */
   Neill_NCURS_CharStyle(sw, "c", COLOR_YELLOW, COLOR_YELLOW, A_BOLD);
   Neill_NCURS_CharStyle(sw, "H", COLOR_BLUE, COLOR_BLUE, A_BOLD);
   Neill_NCURS_CharStyle(sw, "t", COLOR_RED, COLOR_RED, A_BOLD);

   }


void testing(void){

   int rows = 0;
   int columns = 0;
   char arraybefore[SIZE][SIZE];
   char testarray[3][5] = {
      {'t','c','H','j',' '},
      {'/','\0','c','d','e'}
    };

   /* Below is testing to ensure the amendarray func 
   is working as desired. */ 
   for(rows = 0; rows < SIZE; rows++){
      for(columns = 0; columns < SIZE; columns++){
         arraybefore[rows][columns] = ' ';
         }
      }

   arraybefore[0][0] = 'H';
   arraybefore[0][1] = 't';
   arraybefore[0][2] = 'c';
   arraybefore[39][0] = 'H';
   arraybefore[38][1] = 'c';

   /* Testing if a copper with x3 heads surrounding will
   remain as a copper */
   arraybefore[31][10] = 'H';
   arraybefore[29][11] = 'H';
   arraybefore[30][9] = 'H';
   arraybefore[30][10] = 'c';
   
   arrayamend(arraybefore);

   assert(arraybefore[0][0] == 't');
   assert(arraybefore[0][1] == 'c');
   assert(arraybefore[0][2] == 'c');
   assert(arraybefore[38][1] == 'H');
   assert(arraybefore[30][10] == 'c');

   assert(validcharcheck(testarray[0][2]) == True);
   assert(validcharcheck(testarray[0][4]) == True);
   assert(validcharcheck(testarray[0][3]) == False);
   assert(validcharcheck(testarray[1][0]) == False);
   assert(validcharcheck(testarray[1][1]) == False);

   assert(headcheck(testarray[0][3]) == False);
   assert(headcheck(testarray[0][2]) == True);
   assert(headcheck(testarray[0][1]) == False);

   assert(coppercheck(testarray[0][0]) == False);
   assert(coppercheck(testarray[0][1]) == True);
   assert(coppercheck(testarray[0][2]) == False);
   assert(coppercheck(testarray[1][0]) == False);

   assert(charflip('t') == 'c');
   assert(charflip('H') == 't');
   assert(charflip(' ') == '\0');
   assert(charflip('g') == '\0');

   assert(boundarycheck(-1,0) == 0);
   assert(boundarycheck(-1,44) == 0);
   assert(boundarycheck(0,40) == 0);
   assert(boundarycheck(41,0) == 0);
   assert(boundarycheck(40,0) == 0);
   assert(boundarycheck(0,0) == 1);
   assert(boundarycheck(39,39) == 1);

   }
