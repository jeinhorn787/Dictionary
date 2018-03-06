#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	char s[200];
	
	while (fscanf(file, "%s\n", &s[0]) != EOF)
	{
		hashMapPut(map, s, 0);
	}
}



/**
** Levenshtein Distance formula calculates the distance between 2 differen strings. The distance
   between 2 words is the minimum number of single char edits it takes to change one word into
   another.

*  This code is taken from

*  @param s1
*  @param s2
*/
int levenshtein(char *s1, char *s2)
{
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = strlen(s1);
	s2len = strlen(s2);
	int *column = malloc((s1len + 1) * sizeof(int));
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = MIN3(column[y] + 1, column[y - 1] + 1, lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}

	int result = column[s1len];
	free(column);
	return result;
}

void suggestions(HashMap *map, char* arr[])
{
	int i, j;

	for (i = 0; i < map->capacity; i++)
	{
		HashLink *link = map->table[i];

		while (link != NULL)
		{
			if (arr[0] == "")
				arr[0] = link->key;

			else if (*(hashMapGet(map, arr[0])) >= *(hashMapGet(map, link->key)))
			{
				for (j = 5; j > 0; j--)
					arr[j] = arr[j - 1];

				arr[0] = link->key;
			}

			link = link->next;
		}
	}
}


/**
 * Prints the concordance of the given file and performance information. Uses
 * the file input1.txt by default or a file name specified as a command line
 * argument.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);
    
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);
    
    char inputBuffer[256];
    int quit = 0;
	while (!quit)
	{
		printf("Enter a word or \"quit\" to quit: ");
		scanf("%s", inputBuffer);

		// Implement the spell checker code here..
		HashLink *link;
		int i, dist, count = 0;

		if (strcmp(inputBuffer, "quit") == 0)
		{
			quit = 1;
		}

		else if (hashMapContainsKey(map, inputBuffer))
			printf("Spelling is correct.\n");

		else
		{
			char* words[6] = { "" };
			
			for (i = 0; i < map->capacity; i++)
			{
				link = map->table[i];

				while (link != NULL)
				{
					dist = levenshtein(inputBuffer, link->key);
					hashMapPut(map, link->key, dist);
					link = link->next;
				}
			}

			suggestions(map, words);

			for (i = 0; i < 6; i++)
				printf("Did you mean %s?\n", words[i]);
		}
    }
    
    hashMapDelete(map);
    return 0;
}