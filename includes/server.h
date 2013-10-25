#ifndef SERVER__H
#define SERVER__H

#include <stdlib.h>
#include <pthread.h>
#include "../includes/list.h"

// Add macros for accesing methods

struct server
{
	struct string *name;

	int portno;

	pthread_t serve_thread;

	struct list_head pages;

	struct list_head connections;
};

// Add class methods here

struct server *new_server(const char *name, int portno);

void free_server(struct server *server);

#endif
