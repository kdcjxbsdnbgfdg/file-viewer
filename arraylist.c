#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
	int size;
	int capacity;
	char **data;
} stringList;


stringList createStringList(int capacity){
	// allocate space for the data
	char **data = malloc(capacity * sizeof(char *));
	stringList list = {
		0,
		capacity,
		data
	};
	return list;
}

void addStringToList(stringList *list, char *string){
	if (list->size >= list->capacity){
		list->capacity *= 2;
		list->data = realloc(list->data, list->capacity * sizeof(char *));
	}
	// + 1 needed because strlen doesnt count the null terminator
	list->data[list->size] = malloc(strlen(string) + 1);
	strcpy(list->data[list->size], string);
	list->size++;
}

void addStringToListShallow(stringList *list, char *string){
	if (list->size >= list->capacity){
		list->capacity *= 2;
		list->data = realloc(list->data, list->capacity * sizeof(char *));
	}
	list->data[list->size] = string;
	list->size++;
}
