#ifndef <CLASS_NAME>__H
#define <CLASS_NAME>__H

// Add macros for accesing methods

#define FREE_<CLASS_NAME>(x) x->free_<CLASS_NAME>(x)

struct <CLASS_NAME> {
	// Add members here


	// Add methods here
};

// Add class methods here

static struct <CLASS_NAME> *new_<CLASS_NAME>();

static void free_<CLASS_NAME>(struct <CLASS_NAME> *<CLASS_NAME>);

#endif
