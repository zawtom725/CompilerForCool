#include "coolrt.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This file provides the runtime library for cool. It implements
	 the functions of the cool classes in C
	 */

/* Author: Ziang Wan */

/* Class name strings */
const char Object_string[] = "Object";
const char String_string[] = "String";
const char Int_string[] = "Int";
const char Bool_string[] = "Bool";
const char IO_string[] = "IO";
const char default_string[] = "";

/* Class vtable prototypes */

extern const Object_vtable Object_vtable_prototype;
extern const String_vtable String_vtable_prototype;
extern const IO_vtable IO_vtable_prototype;
extern const Int_vtable Int_vtable_prototype;
extern const Bool_vtable Bool_vtable_prototype;

/* ADD CODE HERE FOR MORE VTABLE PROTOTYPES */

/*
	Methods in class object (only some are provided to you)
*/

Object* Object_abort(Object *self) {
	printf("Abort called from class %s\n", !self ? "Unknown" : self->vtable->name);
	exit(1);
	return self;
}

String* Object_type_name(Object *self) {
	if (self == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__): self is NULL\n");
		abort();
	}
	String *s = String_new();
	s->val = self->vtable->name;
	return s;
}

/* ADD CODE HERE FOR MORE METHODS OF CLASS OBJECT */

Object* Object_copy(Object *self){
	// shallow copy
	Object *copy = Object_new();
	copy->vtable = self->vtable;
	return copy;
}

Object* Object_new(void){
	Object *o = malloc(sizeof(Object));
	o->vtable = &Object_vtable_prototype; 
	return o;
}

/*
// Methods in class IO (only some are provided to you)
*/

IO *IO_out_string(IO *self, String *x) {
	if (self == 0 || x == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__): NULL object\n");
		abort();
	}
	printf("%s", x->val);
	return self;
}

IO *IO_out_int(IO *self, Int *x) {
	if (self == 0 || x == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__): NULL object\n");
		abort();
	}
	printf("%d", x->val);
	return self;
}

/*
 * Get one line from stream using get_line(), then discard newline character.
 * Allocate string *in_string_p and store result there.
 * Return number of chars read.
 */
static int get_one_line(char **in_string_p, FILE *stream) {
	/* Get one line worth of input */
	size_t len = 0;
	ssize_t num_chars_read;
	num_chars_read = getline(in_string_p, &len, stdin);
	if (*in_string_p == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__):\n   ");
		fprintf(stderr, "    allocation failed in IO::in_string()\n");
		exit(1);
	}

	/* Discard the newline char, if any.  It may not exist if EOF reached. */
	if (num_chars_read > 0 && (*in_string_p)[num_chars_read - 1] == '\n') {
		(*in_string_p)[num_chars_read - 1] = '\0';
		--len;
	}

	return len;
}

/*
 * The method IO::in_string(): String reads a string from
 * the standard input, up to but not including a newline character.
 */
String *IO_in_string(IO *self) {
	if (self == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__): self is NULL\n");
		abort();
	}

	/* Get one line worth of input with the newline, if any, discarded */
	char *in_string = 0;
	ssize_t len = get_one_line(&in_string, stdin);
	assert(in_string);

	/* We can take advantage of knowing the internal layout of String objects */
	String *str = String_new();
	str->val = in_string;
	return str;
}

/*
 * The method IO::in_int(): Int reads a single integer, which may be preceded
 * by whitespace.
 * Any characters following the integer, up to and including the next newline,
 * are discarded by in_int.
 */
Int *IO_in_int(IO *self) {
	if (self == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__): self is NULL\n");
		abort();
	}

	/* Get one line worth of input with the newline, if any, discarded */
	char *in_string = 0;
	ssize_t len = get_one_line(&in_string, stdin);
	assert(in_string);

	/* Now extract initial int and ignore the rest of the line */
	Int *x = Int_new();
	int num_ints = 0;
	if (len)
		num_ints = sscanf(in_string, " %d", &(x->val)); /* Discards initial spaces*/

	/* If no text found, abort. */
	if (num_ints == 0) {
		fprintf(stderr, "At __FILE__(line __LINE__):\n   ");
		fprintf(stderr, "    Invalid integer on input in IO::in_int()");
		Object_abort((Object *)self);
	}
	return x;
}

/* ADD CODE HERE FOR MORE METHODS OF CLASS IO */

IO* IO_new(void){
	IO* io = malloc(sizeof(IO));
	io->vtable = &IO_vtable_prototype;
	return io;
}

Object* IO_abort(IO* self){
	printf("Abort called from class %s\n", !self ? "Unknown" : self->vtable->name);
	exit(1);
	return Object_new();
}

String* IO_type_name(IO* self){
	String *s = String_new();
	s->val = self->vtable->name;
	return s;
}

IO* IO_copy(IO* self){
	IO* copy = IO_new();
	return copy;
}

/* ADD CODE HERE FOR METHODS OF OTHER BUILTIN CLASSES */

// int class

Int* Int_new(void){
	Int* o = malloc(sizeof(Int));
	o->vtable = &Int_vtable_prototype;
	o->val = 0;
	return o;
}

Object* Int_abort(Int* self){
	printf("Abort called from class %s\n", !self ? "Unknown" : self->vtable->name);
	exit(1);
	return Object_new();
}

String* Int_type_name(Int* self){
	String *s = String_new();
	s->val = self->vtable->name;
	return s;
}

Int* Int_copy(Int* self){
	Int* copy = Int_new();
	copy->val = self->val;
	return copy;
}

// Bool class 

Bool* Bool_new(void){
	Bool* o = malloc(sizeof(Bool));
	o->vtable = &Bool_vtable_prototype;
	o->val = false;
	return o;
}

Object* Bool_abort(Bool* self){
	printf("Abort called from class %s\n", !self ? "Unknown" : self->vtable->name);
	exit(1);
	return Object_new();
}

String* Bool_type_name(Bool* self){
	String *s = String_new();
	s->val = self->vtable->name;
	return s;
}

Bool* Bool_copy(Bool* self){
	Bool* copy = Bool_new();
	copy->val = self->val;
	return copy;
}

// string class

String* String_new(void){
	String* o = malloc(sizeof(String));
	o->vtable = &String_vtable_prototype;
	o->val = NULL;
	return o;
}

Object* String_abort(String* self){
	printf("Abort called from class %s\n", !self ? "Unknown" : self->vtable->name);
	exit(1);
	return Object_new();
}

String* String_type_name(String* self){
	String *s = String_new();
	s->val = self->vtable->name;
	return s;
}

String* String_copy(String* self){
	String* copy = String_new();
	copy->val = self->val;
	return copy;
}

int String_length(String* s) {
	int length = strlen(s->val);
	return length;
}

String* String_concat(String* x, String* s){
	// create a new char array
	char* new_str = malloc(strlen(x->val) + strlen(s->val) + 1);
	memcpy(new_str, x->val, strlen(x->val));
	memcpy(new_str + strlen(x->val), s->val, strlen(s->val));
	new_str[strlen(x->val) + strlen(s->val)] = '\0';

	// create a new object
	String* concated = String_new();
	concated->vtable = x->vtable;
	concated->val = new_str;
	return concated;
}

String* String_substr(String* x, int i, int l){
	// error checking
	if(i >= (int) strlen(x->val)){
		perror("Index out of range");
		exit(1);
	}


	// create a new char array
	char* new_str = malloc(l+1);
	memcpy(new_str, x->val + i, l);
	new_str[l] = '\0';

	// create a new object
	String* sub = String_new();
	sub->vtable = x->vtable;
	sub->val = new_str;
	return sub;
}
