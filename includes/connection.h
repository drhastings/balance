#ifndef CONNECTION__H
#define CONNECTION__H

#include <stdlib.h>
#include "../includes/list.h"
#include "../includes/server.h"

#define BUF_SIZE 8090

// Add macros for accesing methods

struct connection
{
	int sock;

	struct list_head list_item;

	char *start;

	char *end;

	int written;

	struct server *server;

	char buffer[BUF_SIZE];

	void *(*parse_chunk)(struct connection*);

	struct page *page;

	char const *chunk_stop;

	int to_read;
};

// Add class methods here

int is_end_of_chunk(struct connection *conn);

struct connection *new_connection(int sock, struct server *server);

void free_connection(struct connection *connection);

#endif
