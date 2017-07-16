#include <stdio.h>  
#include <string.h> 
#include <stdlib.h> 
#include <time.h>   
#include <ctype.h>  
#include <limits.h> 
#include <stdbool.h>
#include <unistd.h>

#define		UP 		"\e[F" /* Moves cursor up */
#define		BWHITE		"\e[01;37m"
#define		BRED		"\e[01;31m"
#define		BGREEN		"\e[01;32m"
#define		CR		"\e[2K\r"	

/*\
 * 'this': any string
 * 'that': any string
 * Strings must have the same length.
 * return: how many bits the strings differ in
 * side-effects: none
\*/
unsigned string_bit_diff(const char this[], const char that[])
{
	unsigned total = 0; /* the total to be returned */
	
	while (*that && *this) {
		unsigned char diff = *this ^ *that ;
		total += __builtin_popcount(diff);
		++that; ++this;
	}

	return total;
}

/*\
 * get_divergence: get 'divergence' value for two strings.
 * Currently defined as how many bits the strings
 * differ in divided by how many bits they have in total.
 * this: string.
 * that: string.
 * Strings must have the same length.
 * return: the 'divergence'. 
\*/
double get_divergence(const char this[], const char that[])
{
	return 	(double) string_bit_diff(this, that) / 
		(double) (strlen(this) * CHAR_BIT);
}


 /* randomize_string: Randomize all chars in a string.
 *  No control chars.
 *  this: a string.
 *  length: the string's length (excluding '\0') 
 */
void randomize_string(char str[], size_t length)
{
	char random;

	while (length--) {
		do random = (char) rand();
		while (iscntrl(random));
		str[length] = random;
	}
}

int rand_bit(void)
{
	return rand() > RAND_MAX / 2 ? 1 : 0;
}

/* latch: if the character in 'this' is not the same as in 'that', that character
 * gets copied to 'common' */
void latch(const char this[], const char that[], char common[], size_t length)
{
	while (length--) {
		char a = this[length];
		char b = that[length];
		common[length] = (common[length] == a) ? a : b;
	}
}


/* TODO */
double expected_finish_clock(int length)
{
	return 0.0;
}

void print_work(char target[], char found[], char chaos[])
{
	printf( /* Go up one line for every new line in the printed strings */
		UP UP UP
		BGREEN	"FOUND: " BWHITE "%s\n"
		BRED 	"CHAOS: " BWHITE "%s\n"
		CR "DIVERGENCE: %f %%\n",
		found,
		chaos, 
		get_divergence(target, found) * 100.0
	);
}

void print_finish(char target[], time_t start_time, clock_t start_clock)
{
	printf( CR UP UP UP /* delete 'divergence' and 'found' since they're obvious */
		CR "\nDone seeking %s!\nTime spent seeking in...\n"
		CR "- carbon seconds:  %u\n"
		CR "- silicon seconds: %f\n\n",
		target,
		(unsigned) time(0) - (unsigned) start_time,
		(double) (clock() - start_clock) / (double) CLOCKS_PER_SEC);
}

void findloop(char target[])
{
	/* Print the desired target, then as many '\n's as UPs in the next printf plus one */
	size_t length = strlen(target); /* this length applies for all 3 strings. */
	char chaos[length + 1];
	char found[length + 1];
	chaos[length] = '\0';
	found[length] = '\0';
	randomize_string(chaos, length);
	randomize_string(found, length);
	clock_t start_clock = clock();
	time_t start_time = time(NULL);

	printf("TARGT: %s\n\n\n\n", target); 

	/*\ While 'found' and 'target don't match:
	 *	1 - Generate a 'random' string.
	 *	2 - Set in 'found' the chars that 'target and 'random'
	 *	  have in common. These chars stay permanently in 'found'.
	 *	3 - Calculate the 'divergence'.
	 *	4 - Print everything done above. 
	\*/
	while (strcmp(found, target)) {

		randomize_string(chaos, length);
		latch(target, chaos, found, length);
		print_work(target, found, chaos);

		/* Of course we have to sleep or the future gadget would overheat. */
		#if !FAST	
		usleep(66666); /* 66666 microseconds period <=> 15 Hz frequency */
		#endif
	}
	print_finish(target, start_time, start_clock);
}

void stdinloop(void)
{
	char* target = NULL;
	size_t size = 0;
	while (getline(&target, &size, stdin) > 0) {
		target[strlen(target) - 1] = '\0';
		findloop(target);
	}	
	free(target);
}

void argloop(char* argv[])
{
	char* target; 			/* the string to be found, entered by the user */
	while ((target = *++argv)) 	/* Walk through all strings in 'argv', except for the zeroth */
		findloop(target);
}

void print_intro(void)
{
	printf( BWHITE
		"FUTURE GADGET 451: CHAOS SEEKER\n"
		"BEGIN SEEKING AT FULL POWER\n"
		"ONLINE TRANSISTORS: %u\n\n",
		0xDEADBEEF);
}

int main(int argc, char* argv[])
{	
	srand(clock());
	print_intro();
	if (argc > 1)
		argloop(argv);
	else 
		stdinloop();

	return EXIT_SUCCESS;
}
