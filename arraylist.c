#include <stdlib.h>
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
	list->data[list->size] = string;
	list->size++;
}
