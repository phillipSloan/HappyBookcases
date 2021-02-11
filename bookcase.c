#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define NUMALPHA 26
#define MAX 9
#define ARRAYSIZE 10000000
#define END (ARRAYSIZE - (MAX*(MAX-1)))
#define STRLEN 100
#define INITIALBOOK -1

typedef enum bool {false, true} bool;

enum colours {black = 'K', red = 'R', green = 'G',
              yellow = 'Y', blue = 'B', magenta = 'M',
              cyan = 'C', white = 'W', empty = '.'};
typedef enum colours colours;

typedef struct bks {
   char bookcase[MAX][MAX];
   int height;
   int width;
   int parent;
   int NumOfMoves;
} Bks;

typedef struct lib {
   struct bks* bookcases;
   int start;
   int end;
   int count;
} Lib;

/*test functions*/
void test(void);
int test_checkRearrange(Lib* library, Bks bk);
/*generation functions*/
void isHappy(Lib* library);
bool isBookcaseHappy(Bks bk);
/*main algorithm*/
bool rearrange(Lib* library, Bks bk, int parentIndex);
Bks makeChild(Bks parent, int shelf1, int shelf2, int parentIndex);
bool isShelfHappy(Bks bk, int shelf);
bool checkDifferentShelves(Bks bk);
bool checkBookIsValidType(char book);
bool isBook(Bks bk, int shelf, int book);
bool isFree(Bks bk, int shelf, int book);
int findRightMostBookOnShelf(Bks bk, int shelf);
int findFirstSpaceOnShelf(Bks bk, int shelf);
bool hasSpace(Bks bk, int shelf);
void printBookcase(Bks *bk);
void swap(char* a, char* b);
void on_error(const char* s);
void printSolution(Lib* library, int loc);
/*functions to make list*/
Lib* makeLibrary(void);
void addToLibrary(Lib *library, Bks newBk);
bool freeLibrary(Lib *library);
/*file functions and functions to test input file is valid*/
Bks readFile(char* argv);
bool extractFirstLine(FILE* fp, Bks* initialBook);
bool isValidHeightWidth(char* str);
bool extractBookcase(FILE* fp, Bks* initialBook);
void testFile(Bks initialBook);
bool checkValidBooks(Bks bk);
bool isSolvable(Bks bk);

int main(int argc, char* argv[])
{
   int loc;
   Bks initialBook;
   Lib* library = NULL;

   test();

   library = makeLibrary();

   if((argc == 2) || (argc == 3)){
      initialBook = readFile(argv[1]);
      addToLibrary(library, initialBook);

      isHappy(library);
      loc = library->end - 1;
      printf("%i\n", library->bookcases[loc].NumOfMoves);

      if ((argc > 2) && (strcmp(argv[2], "verbose") == 0)){
         printSolution(library, loc);
      }
   } else {
      on_error("Incorrect command line arguments");
   }
   freeLibrary(library);
   return 0;
}

void isHappy(Lib* library)
{
   int gen = 0;
   bool solutionFound = false;
   while (!isBookcaseHappy(library->bookcases[gen]) &&
          !solutionFound){
      solutionFound = rearrange(library,\
                      library->bookcases[gen], gen);
      gen++;

      if (library->end > END){
         printf("No Solution?");
         freeLibrary(library);
         exit(EXIT_FAILURE);
      }
   }
}

bool isBookcaseHappy(Bks bk)
{
   int shelf;
   for (shelf = 0; shelf < bk.height; shelf++){
      if (!isShelfHappy(bk, shelf) ||
         !checkDifferentShelves(bk)){
         return false;
      }
   }
   return true;
}

bool isShelfHappy(Bks bk, int shelf)
{
   int book = 0;
   char firstBook, currentBook;

   firstBook = bk.bookcase[shelf][book];
   while (book < bk.width){
      currentBook = bk.bookcase[shelf][book];
      if ((firstBook != currentBook) &&
         (currentBook != empty)){
         return false;
      }
      book++;
   }
   return true;
}

bool checkDifferentShelves(Bks bk)
{
   char book1, book2;
   int shelf1, shelf2, firstBookOnShelf = 0;
   for (shelf1 = 0; shelf1 < bk.height; shelf1++){
      book1 = bk.bookcase[shelf1][firstBookOnShelf];
      /*Ensuring I only check books*/
      if(checkBookIsValidType(book1)){
         /*already checked shelf1 against other shelves*/
         for (shelf2 = shelf1; shelf2 < bk.height; shelf2++){
            book2 = bk.bookcase[shelf2][firstBookOnShelf];
            /*Ensuring I only check books*/
            if(checkBookIsValidType(book2)){
               if ((shelf1 != shelf2) && (book1 == book2)){
                  return false;
               }
            }
         }
      }
   }
   return true;
}

bool rearrange(Lib* library, Bks parent, int parentIndex)
{
   int shelf, shelf2, firstSpaceOnShelf = 0;
   Bks child;
   library->count = 0;

   if (isBookcaseHappy(parent)){
      return true;
   }

   for (shelf = 0; shelf < parent.height; shelf++){
      /*If there is a book on the first space of a shelf*/
      if (isBook(parent, shelf, firstSpaceOnShelf)){
         for (shelf2 = 0; shelf2 < parent.height; shelf2++){
            /*If it's not the same shelf and there is space*/
            if ((shelf != shelf2) && hasSpace(parent, shelf2)){
               child = makeChild(parent, shelf, shelf2, parentIndex);
               addToLibrary(library, child);
               library->count = library->count + 1;
               /*check if it's the solution, stop if so*/
               if (isBookcaseHappy(child)){
                  return true;
               }
            }
         }
      }
   }
   return false;
}

Bks makeChild(Bks parent, int shelf, int shelf2, int parentIndex)
{
   Bks child;
   int firstEmpty, lastBook;

   lastBook = findRightMostBookOnShelf(parent, shelf);
   firstEmpty = findFirstSpaceOnShelf(parent, shelf2);

   child = parent;
   child.NumOfMoves = child.NumOfMoves + 1;
   child.parent = parentIndex;
   swap(&child.bookcase[shelf][lastBook],\
       &child.bookcase[shelf2][firstEmpty]);

   return child;
}

void printSolution(Lib* library, int loc)
{
   while (loc > INITIALBOOK){
     printBookcase(&library->bookcases[loc]);
     loc = library->bookcases[loc].parent;
  }
}


Lib* makeLibrary(void)
{
   Lib* library = (Lib*)calloc(1, sizeof(Lib));
   if (library == NULL){
      on_error("Not enough memory");
   }
   library->bookcases = (Bks*)calloc(ARRAYSIZE, sizeof(Bks));
   if (library->bookcases == NULL){
      on_error("Not enough memory");
   }
   return library;
}

void addToLibrary(Lib* library, Bks bk)
{
   if (library){
      library->bookcases[library->end] = bk;
      library->end = library->end + 1;
   }
}

bool freeLibrary(Lib *library){
   if(library == NULL){
      return true;
   }

   if(library->bookcases) {
      free(library->bookcases);
   }
   free(library);
   return true;
}

void swap(char* a, char* b)
{
   char tmp;
   tmp = *a;
   *a = *b;
   *b = tmp;
}

int findRightMostBookOnShelf(Bks bk, int shelf)
{
   int book;
   char currentBook = empty;
   for (book = (bk.width - 1); book >= 0; book--){
      currentBook = bk.bookcase[shelf][book];
      if (currentBook != empty){
         return book;
      }
   }
   return 0;
}

int findFirstSpaceOnShelf(Bks bk, int shelf)
{
   int space = 0;
   while (((space < bk.width - 1)) && isBook(bk, shelf, space)){
      space++;
   }
   return space;
}

bool hasSpace(Bks bk, int shelf)
{
   int endOfShelf = bk.width - 1;
   if (bk.bookcase[shelf][endOfShelf] == empty){
      return true;
   }
   return false;
}

bool checkBookIsValidType(char book)
{
  if ((book == black)   ||
      (book == red)     ||
      (book == green)   ||
      (book == yellow)  ||
      (book == white)   ||
      (book == magenta) ||
      (book == cyan)    ||
      (book == blue)){
      return true;
   }
   return false;
}

bool isBook(Bks bk, int shelf, int book)
{
   char currentBook = bk.bookcase[shelf][book];
   return checkBookIsValidType(currentBook);
}

bool isFree(Bks bk, int shelf, int book)
{
   return (bk.bookcase[shelf][book] == empty);
}

void printBookcase(Bks *bk)
{
   int shelf, book;
   printf("\n");
   for (shelf = 0; shelf < bk->height; shelf++){
      for (book = 0; book < bk->width; book++){
         printf("%c", bk->bookcase[shelf][book]);
      }
      printf("\n");
   }
   printf("\n");
}

Bks readFile(char* argv)
{
   int start = 1;
   Bks initialBook;
   FILE *fp;
   initialBook.NumOfMoves = start;
   initialBook.parent = INITIALBOOK;

   if ((fp = fopen(argv, "r")) == NULL){
      on_error("Unable to open file");
   }

   if (!extractFirstLine(fp, &initialBook)){
      on_error("Invalid height and/or width");
   }

   if (!extractBookcase(fp, &initialBook)){
      on_error("Height/width doesn't match bookcase dimensions");
   }
   fclose(fp);
   testFile(initialBook);
   return initialBook;
}

bool extractFirstLine(FILE* fp, Bks* initialBook)
{
   char *token = NULL, *space = " ";
   int tokCtr = 0, height = 0, width = 1;
   char str[STRLEN] = {0};

   if (fgets(str, STRLEN, fp) != NULL){
      token = strtok(str, space);
      while (token != NULL){
         if (tokCtr == height){
            if (isValidHeightWidth(token)){
               initialBook->height = atoi(token);
            } else {
               return false;
            }
         } else if (tokCtr == width){
            if (isValidHeightWidth(token)){
               initialBook->width = atoi(token);
            } else {
               return false;
            }
         }
         tokCtr++;
         token = strtok(NULL, space);
      }
   }
   return true;
}

bool isValidHeightWidth(char* str)
{
   int firstChar = 0;
   char minMazeSize = '1', maxMazeSize = '9';
   return (str[firstChar] >= minMazeSize &&
           str[firstChar] <= maxMazeSize);
}

bool extractBookcase(FILE* fp, Bks* initialBook)
{
   int shelf = 0, book, len;
   char str[STRLEN] = {0};
   while ((shelf < initialBook->height) &&
         (fscanf(fp, "%s", str) != EOF)){
      len = strlen(str);
      if (len > initialBook->width){
         return false;
      }
      for (book = 0; book < initialBook->width; book++){
         initialBook->bookcase[shelf][book] = toupper(str[book]);
      }
      shelf++;
      if (shelf > initialBook->height){
         return false;
      }
   }
   return true;
}

void testFile(Bks initialBook)
{
   if (!checkValidBooks(initialBook)){
      on_error("File contains invalid book types.");
   }

   if (!isSolvable(initialBook)){
      fprintf(stderr, "No Solution?\n");
      exit(EXIT_SUCCESS);
   }
}

bool isSolvable(Bks bk)
{
   int alpha[NUMALPHA] = {0};
   int shelf, book, i, countOfBookTypes = 0;
   for (shelf = 0; shelf < bk.height; shelf++){
      for (book = 0; book < bk.width; book++){
         if (isBook(bk, shelf, book)){
            ++alpha[(bk.bookcase[shelf][book] - 'A')];
         }
      }
   }
   for (i = 0; i < NUMALPHA; i++){
      if (alpha[i] > 0){
         countOfBookTypes++;
      }
      if (alpha[i] > bk.width){
         return false;
      }
   }
   if (countOfBookTypes > bk.height){
      return false;
   }
   return true;
}

bool checkValidBooks(Bks bk)
{
   int shelf, book;
   for (shelf = 0; shelf < bk.height; shelf++){
      for (book = 0; book < bk.width; book++){
         /*if it's not a book or a free space*/
         if (!isBook(bk, shelf, book) &&
             !isFree(bk, shelf, book)){
            return false;
         }
      }
   }
   return true;
}

/*From general.c of the ADTs material*/
void on_error(const char* s)
{
   fprintf(stderr, "%s\n", s);
   exit(EXIT_FAILURE);
}


void test(void)
{
   int shelf, book, mostChildren;
   Lib *library = NULL;
   Bks testBookcase = {{"KRG",
                        "YBM",
                        "CWK"}, 3, 3, -1, 0};

   Bks testBookcase2 = {{"KG.",
                         "...",
                         "MMM"}, 3, 3, -1, 0};

   Bks testBookcase3 = {{"Y..",
                         "BBY",
                         "YB."}, 3, 3, -1, 0};

   Bks testBookcase4 = {{"YY.",
                         "BB.",
                         "Y.."}, 3, 3, -1, 0};

   Bks emptyBookcase = {{"...",
                         "...",
                         "..."}, 3, 3, -1, 0};


   Bks invalidBookcase = {{",@%£$%l3",
                           "(xdpq-75"}, 1, 9, -1, 0};

   Bks happyBookcase = {{"YYY",
                         "BBB",
                         "..."}, 3, 3, -1, 0};

   Bks histoBookcase = {{"RG...",
                         "G....",
                         "CYR..",
                         "YC...",
                         "....."}, 5, 5, -1, 0};

   for (shelf = 0; shelf < 3; shelf++){
      for (book = 0; book < 3; book++){
         assert(isBook(testBookcase, shelf, book));
         assert(isFree(emptyBookcase, shelf, book));
      }
   }
   for (shelf = 0; shelf < 2; shelf++){
      for (book = 0; book < 9; book++){
         assert(!isBook(invalidBookcase, shelf, book));
      }
   }

   for (shelf = 0; shelf < 3; shelf++){
      assert(isShelfHappy(happyBookcase, shelf));
      assert(isShelfHappy(emptyBookcase, shelf));
   }

   assert(isBookcaseHappy(happyBookcase));
   assert(isBookcaseHappy(emptyBookcase));
   assert(!isBookcaseHappy(testBookcase));
   assert(!isBookcaseHappy(testBookcase4));

   assert(checkDifferentShelves(happyBookcase));
   assert(checkDifferentShelves(emptyBookcase));
   assert(!checkDifferentShelves(testBookcase4));

   assert(findRightMostBookOnShelf(testBookcase2, 0) == 1);
   assert(findRightMostBookOnShelf(testBookcase2, 1) == 0);
   assert(findRightMostBookOnShelf(testBookcase2, 2) == 2);

   assert(findFirstSpaceOnShelf(testBookcase2, 0) == 2);
   assert(findFirstSpaceOnShelf(testBookcase2, 2) == 2);

   assert(hasSpace(testBookcase2, 0));
   assert(hasSpace(testBookcase2, 1));
   assert(!hasSpace(testBookcase2, 2));

   assert(!isSolvable(testBookcase));
   assert(isSolvable(testBookcase2));

   assert(checkValidBooks(testBookcase));
   assert(checkValidBooks(emptyBookcase));
   assert(!checkValidBooks(invalidBookcase));


   library = makeLibrary();
   assert(library);

   assert(rearrange(library, happyBookcase, 0));
   rearrange(library, testBookcase2, 0);
   /*this only requires one child to make a happy bookcase*/
   assert(library->count == 1);

   /*
     Ensuring rearrange function does not perform
     more than height*height-1 moves on a bookcase
   */
   mostChildren = test_checkRearrange(library, histoBookcase);
   assert(mostChildren <= histoBookcase.height*\
                         (histoBookcase.height - 1));

   mostChildren = test_checkRearrange(library, testBookcase3);
   assert(mostChildren <= testBookcase3.height*\
                         (testBookcase3.height - 1));
   if(library){
      assert(freeLibrary(library));
   }
}

int test_checkRearrange(Lib* library, Bks bk)
{
   int histoMoves[72] = {0};
   int i, mostChildren = 0, highestMoves = 0;
   library->count = 0;

   for (i = 0; i < 500; i++){
      rearrange(library, bk, i);
      ++histoMoves[library->count];
   }

   for(i = 0; i < 72; i++){
      if (histoMoves[i] > highestMoves){
         mostChildren = i;
         highestMoves = histoMoves[i];
      }
   }
   return mostChildren;
}
