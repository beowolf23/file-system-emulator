#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir
{
	char *name;
	struct Dir *parent;
	struct File *head_children_files;
	struct Dir *head_children_dirs;
	struct Dir *next;
} Dir;

typedef struct File
{
	char *name;
	struct Dir *parent;
	struct File *next;
} File;

// functie pentru crearea si alocarea memoriei pentru un director
Dir *create_dir(Dir *parent, char *name)
{

	Dir *new_directory = (Dir *) malloc(sizeof(Dir));
	new_directory->name = malloc(sizeof(name));
	new_directory->head_children_files = NULL;
	new_directory->head_children_dirs = NULL;
	new_directory->parent = parent;
	new_directory->next = NULL;
	strcpy(new_directory->name, name);
	return new_directory;
}

// functie pentru eliberarea memoriei unui director (nerecursiv)
void free_dir(Dir *directory)
{

	free(directory->name);
	free(directory);
}

// functie pentru crearea si alocarea unui fisier
File *create_file(Dir *parent, char *name)
{

	File *new_file = (File *) malloc(sizeof(File));
	new_file->name = malloc(sizeof(name));
	new_file->next = NULL;
	new_file->parent = parent;
	strcpy(new_file->name, name);
	return new_file;
}

// functie pentru eliberarea memoriei unui fisier
void free_file(File *file)
{

	free(file->name);
	free(file);
}

// functie pentru eliberarea memoriei unui director (recursiv)
void free_all(Dir *target)
{

	Dir *dir = target->head_children_dirs;
	Dir *dir_next;
	File *file = target->head_children_files;
	File *file_next;
	free_dir(target);
	/*
	 * se elibereaza memoria fiecarui fisier din ierarhia de directoare
	 * ce porneste din target
	 */
	while (file)
	{
		file_next = file->next;
		free_file(file);
		file = file_next;
	}
	/*
	 * se elibereaza memoria fiecarui director din ierarhia de directoare
	 * ce porneste din target
	 */
	while (dir)
	{
		dir_next = dir->next;
		free_all(dir);
		dir = dir_next;
	}
}

// functie pentru adaugarea unui fisier in directorul target
void touch(Dir *parent, char *name)
{

	File *iterator = parent->head_children_files;
	File *iterator_prev = iterator;
	// daca directorul nu contine niciun fisier se adauga head-ul
	if (!iterator)
	{
		parent->head_children_files = create_file(parent, name);
	}
		// daca directorul contine fisiere se adauga la final
	else
	{
		while (iterator)
		{
			if (strcmp(iterator->name, name) == 0)
			{
				printf("File already exists\n");
				return;
			}
			iterator_prev = iterator;
			iterator = iterator->next;
		}
		iterator_prev->next = create_file(parent, name);
		return;
	}
}

// functie pentru crearea unui subdirector al target-ului
void mkdir(Dir *parent, char *name)
{

	Dir *iterator = parent->head_children_dirs;
	Dir *iterator_prev;
	// daca target nu contine directoare se adauga head-ul
	if (!iterator)
	{
		parent->head_children_dirs = create_dir(parent, name);
	}
		// daca target contine alte directoare se adauga la final
	else
	{
		while (iterator)
		{
			if (strcmp(iterator->name, name) == 0)
			{
				printf("Directory already exists\n");
				return;
			}
			iterator_prev = iterator;
			iterator = iterator->next;

		}
		Dir *dir = create_dir(parent, name);
		iterator_prev->next = dir;
	}
}

// functie pentru listarea continutului directorului target
void ls(Dir *parent)
{

	Dir *iterator_dir = parent->head_children_dirs;
	File *iterator_file = parent->head_children_files;
	// iterez prin fiecare subdirector si afisez numele
	while (iterator_dir)
	{
		printf("%s\n", iterator_dir->name);
		iterator_dir = iterator_dir->next;
	}
	// iterez prin fiecare fisier si afisez numele
	while (iterator_file)
	{
		printf("%s\n", iterator_file->name);
		iterator_file = iterator_file->next;
	}
}

// functie pentru stergerea unui fisier din directorul target
void rm(Dir *parent, char *name)
{

	File *iterator = parent->head_children_files;
	File *iterator_prev = iterator;
	// daca nu exista fisiere in lista de fisiere din director
	if (iterator == NULL)
	{
		printf("Could not find the file\n");
		return;
	}
	// daca fisierul cautat e head-ul listei de fisiere
	if (iterator != NULL && strcmp(iterator->name, name) == 0)
	{
		parent->head_children_files = iterator->next;
		free_file(iterator);
		return;
	}
	// caut fisierul in lista de fisiere si il sterg daca se gaseste
	while (iterator != NULL)
	{
		if (strcmp(iterator->name, name) == 0)
		{
			if (iterator->next)
			{
				iterator_prev->next = iterator->next;
				free_file(iterator);
				return;
			}
			else
			{
				iterator_prev->next = NULL;
				free_file(iterator);
				return;
			}
		}
		iterator_prev = iterator;
		iterator = iterator->next;
	}
	// daca fisierul nu a fost gasit in lista de fisiere
	if (!iterator)
	{
		printf("Could not find the file\n");
	}
}

//functie pentru stergerea unui subdirector al directorului target
void rmdir(Dir *parent, char *name)
{

	Dir *iterator = parent->head_children_dirs;
	Dir *iterator_prev = iterator;
	// daca directorul target nu contine subdirectoare
	if (iterator == NULL)
	{
		printf("Could not find the dir\n");
		return;
	}
	// daca directorul cautat este fix head-ul
	if (iterator != NULL && strcmp(iterator->name, name) == 0)
	{
		parent->head_children_dirs = iterator->next;
		free_all(iterator);
		return;
	}
	// caut in lista directorul pe care vreau sa il sterg
	while (iterator != NULL)
	{
		if (strcmp(iterator->name, name) == 0)
		{
			if (iterator->next)
			{
				iterator_prev->next = iterator->next;
				free_all(iterator);
				return;
			}
			else
			{
				iterator_prev->next = NULL;
				free_all(iterator);
				return;
			}
		}
		iterator_prev = iterator;
		iterator = iterator->next;
	}
	// daca nu s-a gasit directorul cautat
	if (!iterator || strcmp(iterator->name, name) != 0)
	{
		printf("Could not find the dir\n");
	}
}

// functie pentru navigarea intre directoare
void cd(Dir **target, char *name)
{
	// navighez spre parent-ul directorului target
	if (strcmp(name, "..") == 0)
	{
		if (strcmp((*target)->name, "home") == 0)
		{
			return;
		}
		*target = (*target)->parent;
		return;
	}
		// caut in lista de subdirectoare si schimb directorul curent
	else
	{
		Dir *iterator = (*target)->head_children_dirs;
		while (iterator)
		{
			if (strcmp(iterator->name, name) == 0)
			{
				*target = iterator;
				return;
			}
			iterator = iterator->next;
		}
		printf("No directories found!\n");
	}

}

// functie pentru printarea path-ului catre directorul curent
char *pwd(Dir *target)
{

	char *path_reversed = calloc(MAX_INPUT_LINE_SIZE / 2, 1);
	char *path = calloc(MAX_INPUT_LINE_SIZE / 2, 1);
	char **array_of_strings;
	int index = 0;
	array_of_strings = malloc(sizeof(char *));
	// daca directorul curent este home
	if (strcmp(target->name, "home") == 0)
	{
		strcat(path, "/home");
	}
		// introduc fiecare nume al directoarelor in array-ul de string-uri
	else
	{
		do
		{
			array_of_strings[index] = malloc(strlen(target->name) + 1);
			strcpy(array_of_strings[index], target->name);
			index += 1;
			array_of_strings = realloc(array_of_strings, (index + 1) * sizeof(char *));
			cd(&target, "..");
		} while (strcmp(target->name, "home") != 0);
		// concatenez string-urile din array in noul char *
		strcat(path, "/home");
		for (int i = index - 1; i >= 0; i--)
		{
			strcat(path, "/");
			strcat(path, array_of_strings[i]);
		}
	}
	// eliberez memoria array-ului
	for (int i = 0; i < index; i++)
	{
		free(array_of_strings[i]);
	}
	free(array_of_strings);
	free(path_reversed);
	return path;
}

// functie recursiva pentru printarea continutului ierarhiei de directoare
void tree(Dir *target, int level)
{

	int i;
	Dir *dir = target->head_children_dirs;
	File *file = target->head_children_files;

	// printez directoarele recursiv
	while (dir)
	{
		for (i = 0; i < 4 * level; i++)
		{
			printf(" ");
		}
		printf("%s\n", dir->name);
		tree(dir, level + 1);
		dir = dir->next;
	}

	// pentru fiecare director printez fisierele din lista de fisiere
	while (file)
	{
		for (i = 0; i < 4 * level; i++)
		{
			printf(" ");
		}
		printf("%s\n", file->name);
		file = file->next;
	}

}

/* functie pentru eliberarea memoriei structurilor de date (recursiv)
 * si iesirea din program */

void stop(Dir *target)
{

	free_all(target);
	exit(0);
}

/* functie pentru redenumirea unui fisier/director si mutarea
 * acestuia la finalul listei*/

void mv(Dir *parent, char *oldname, char *newname)
{

	Dir *dir = parent->head_children_dirs;
	File *file = parent->head_children_files;

	File *iterator_file = file;
	Dir *iterator_dir = dir;

	//daca noul nume de fisier exista deja
	while (iterator_file)
	{
		if (strcmp(iterator_file->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		iterator_file = iterator_file->next;
	}

	// daca noul nume de director exista deja
	while (iterator_dir)
	{
		if (strcmp(iterator_dir->name, newname) == 0)
		{
			printf("File/Director already exists\n");
			return;
		}
		iterator_dir = iterator_dir->next;
	}

	// se sterge directorul pe care vrem sa il redenumim si se adauga la final
	while (dir)
	{
		if (strcmp(dir->name, oldname) == 0)
		{
			rmdir(parent, oldname);
			mkdir(parent, newname);
			return;
		}
		dir = dir->next;
	}

	// se sterge fisierul pe care vrem sa il redenumim si se adauga la final
	while (file)
	{

		if (strcmp(file->name, oldname) == 0)
		{
			rm(parent, oldname);
			touch(parent, newname);
			return;
		}
		file = file->next;
	}
	// daca nu s-a gasit fisierul/directorul pe care vrem sa il redenumim
	printf("File/Director not found\n");
}

void main()
{

	char *line = malloc(MAX_INPUT_LINE_SIZE);
	char *command = NULL, *name = NULL, *name_new = NULL;
	Dir *home = create_dir(NULL, "home");
	Dir *current = home;

	do
	{
		fgets(line, MAX_INPUT_LINE_SIZE, stdin);
		char *token = strtok(line, " ");

		//impart linia citita in token-uri (command, name, name_new)
		if (token)
		{
			command = malloc(strlen(token) + 1);
			strcpy(command, token);
			command[strlen(command)] = '\0';
			token = strtok(NULL, " \n");
			if (token)
			{
				name = calloc(strlen(token) + 1, 1);
				strcpy(name, token);
				name[strlen(name)] = '\0';
				token = strtok(NULL, " \n");
				if (token)
				{
					name_new = calloc(strlen(token) + 1, 1);
					strcpy(name_new, token);
					name_new[strlen(name_new)] = '\0';
				}
			}
		}

		// verific daca comanda data exista, o aplic si eliberez memoria
		if (strcmp(command, "touch") == 0 || strcmp(command, "touch\n") == 0)
		{
			touch(current, name);
			free(command);
			free(name);
			continue;
		}

		if (strcmp(command, "mkdir") == 0 || strcmp(command, "mkdir\n") == 0)
		{
			mkdir(current, name);
			free(command);
			free(name);
			continue;
		}

		if (strcmp(command, "ls") == 0 || strcmp(command, "ls\n") == 0)
		{
			ls(current);
			free(command);
			continue;
		}

		if (strcmp(command, "rm") == 0 || strcmp(command, "rm\n") == 0)
		{
			rm(current, name);
			free(command);
			free(name);
			continue;
		}

		if (strcmp(command, "rmdir") == 0 || strcmp(command, "rmdir\n") == 0)
		{
			rmdir(current, name);
			free(command);
			free(name);
			continue;
		}

		if (strcmp(command, "cd") == 0 || strcmp(command, "cd\n") == 0)
		{
			cd(&current, name);
			free(command);
			free(name);
			continue;
		}

		if (strcmp(command, "pwd") == 0 || strcmp(command, "pwd\n") == 0)
		{
			char *working_dir = pwd(current);
			printf("%s", working_dir);
			free(working_dir);
			free(command);
			continue;
		}

		if (strcmp(command, "tree") == 0 || strcmp(command, "tree\n") == 0)
		{
			tree(current, 0);
			free(command);
			continue;
		}

		if (strcmp(command, "mv") == 0 || strcmp(command, "mv\n") == 0)
		{
			mv(current, name, name_new);
			free(command);
			free(name);
			free(name_new);
			continue;
		}

		if (strcmp(command, "stop") == 0 || strcmp(command, "stop\n") == 0)
		{
			free(command);
			free(line);
			stop(home);
		}

		free(command);

	} while (1);

}
