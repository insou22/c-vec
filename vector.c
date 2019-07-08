/**
 * C-Vec
 *
 * This program was written by Zac Kologlu (z5257261)
 * on 2019-03-18
 *
 * This program is documented fully with JavaDoc style documentation.
 * A static HTML doc website can be generated with a tool such as Doxygen for C,
 * or it can simply be read inline with the code.
 *
 * C-Vec is a self expanding / reallocating vector library.
 * It includes a handful of relatively simple
 * builtin functions in order to manage the vector.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>


//////////////////////////////////////////////////////
//				 // DEFINITIONS //					//
//////////////////////////////////////////////////////
#define TRUE 					1					//
#define FALSE 					0					//
#define BOOL 					int					//
//////////////////////////////////////////////////////


/**
 * Vector struct.
 *
 * @param *array The pointer representing the current array on the heap
 * @param elem_size The size (in bytes) of each element
 * @param length The amount of elements currently stored in the array (Default: 0)
 * @param capacity How many elements the Vector is currently able to hold
 */
typedef struct Vector {
	void *array;
	size_t elem_size;
	size_t length;
	size_t capacity;
} Vector;


/**
 * The nearest power of 2 from x upwards.
 *
 * @param x The number to operate on
 * @return The integer n for which 2^n >= x and is the lowest value of this form
 */
int next_pow_2(int x);

/**
 * Create a new vector with default size 16
 *
 * @param elem_size The size of each element in the vector
 * @return The generated vector
 */
Vector* create_vector(size_t elem_size);

/**
 * Creates a new vector and fills it with initial_size default values of default_value.
 *
 * @param elem_size The size of each element in the vector
 * @param initial_size The initial size to create the vector with default values
 * @param default_value The default value to fill the vector with
 * @return The generated vector
 */
Vector* create_vector_with_default(size_t elem_size, size_t initial_size, void *default_value);

/**
 * Creates a new vector with *at least* initial_size capacity (the next power of 2 for ease of management).
 *
 * @param elem_size The size of each element in the vector
 * @param initial_size The capacity at which the vector will have a minimum of
 * @return The generated vector
 */
Vector* create_vector_with_capacity(size_t elem_size, size_t initial_size);

/**
 * Clones a vector into new memory.
 *
 * @param old The old vector to copy from
 * @return The new copied vector
 */
Vector* clone(Vector* old);

/**
 * Gets the element at a specific index of the vector.
 *
 * @param vector The vector
 * @param index The index to retrieve the element from
 * @return The value as void*
 */
void* get_elem(Vector *vector, int index);

/**
 * Sets an element at a particular index to a given value
 *
 * @param vector The vector
 * @param index The index to set the element of
 * @param element The data to set the element to
 */
void set_elem(Vector *vector, int index, void *element);

/**
 * Removes an element from a vector and shifts the rest of the vector across.
 *
 * @param vector The vector
 * @param index The index of the item to remove
 */
void remove_elem(Vector *vector, int index);

/**
 * Searches a vector and returns the index of where a given element lies.
 *
 * @param vector The vector to search
 * @param element The data to search for
 * @return The index of the found element, or -1 if it does not exist
 */
int index_of(Vector *vector, void *element);

/**
 * Whether the vector is currently empty.
 *
 * @param vector The vector
 * @return Whether it is empty
 */
BOOL is_empty(Vector *vector);

/**
 * Whether any elements are common between two vectors.
 *
 * @param vector1 The first vector
 * @param vector2 The second vector
 * @return Whether any elements are the same
 */
BOOL any_elems_shared(Vector *vector1, Vector *vector2);

/**
 * Swaps two elements in a vector from given indexes.
 *
 * @param vector The vector to perform the swap in
 * @param index1 The first index
 * @param index2 The second index
 */
void swap_elems(Vector *vector, int index1, int index2);


/**
 * Sorts a vector with a comparison function.
 * Uses selection sort (O(n^2)).
 *
 * @param vector The vector to sort
 * @param sort  The function pointer to compare two similar elements
 *              (Return 1 if elem1 > elem2, 0 if elem1 == elem2, -1 if elem1 < elem2)
 */
void sort_vector(Vector *vector, int (*sort)(void *elem1, void *elem2));

/**
 * Push an element to the back of the vector.
 *
 * @param vector The vector
 * @param element The element to insert
 */
void push_back(Vector *vector, void *element);

/**
 * Expands a vector to a new capacity (retaining all current data)
 *
 * @param vector The vector
 * @param new_size The size to expand the vector's capacity to
 */
void expand_vector(Vector *vector, size_t new_size);

/**
 * Memory management: Deallocate a vector.
 *
 * @param vector The vector to deallocate
 */
void free_vector(Vector *vector);



/**
 * The nearest power of 2 from x upwards.
 *
 * @param x The number to operate on
 * @return x if x is a power of 2, otherwise the next power of 2 greater than x
 */
int next_pow_2(int x) {
	if (x <= 2) {
		return 1;
	}
	double log2 = log(x) / log(2);
	return (int) ceil(log2);
}

/**
 * Create a new vector with default size 16
 *
 * @param elem_size The size of each element in the vector
 * @return The generated vector
 */
Vector* create_vector(size_t elem_size) {
	return create_vector_with_capacity(elem_size, 16);
}

/**
 * Creates a new vector and fills it with initial_size default values of default_value.
 *
 * @param elem_size The size of each element in the vector
 * @param initial_size The initial size to create the vector with default values
 * @param default_value The default value to fill the vector with
 * @return The generated vector
 */
Vector* create_vector_with_default(size_t elem_size, size_t initial_size, void *default_value) {
	Vector *vector = create_vector_with_capacity(elem_size, initial_size);

	for (int i = 0; i < initial_size; i++) {
		push_back(vector, default_value);
	}

	return vector;
}

/**
 * Creates a new vector with *at least* initial_size capacity (the next power of 2 for ease of management).
 *
 * @param elem_size The size of each element in the vector
 * @param initial_size The capacity at which the vector will have a minimum of
 * @return The generated vector
 */
Vector* create_vector_with_capacity(size_t elem_size, size_t initial_size) {
	size_t actual_size = (size_t) pow(2, next_pow_2(initial_size));

	void *array = calloc(actual_size, elem_size);

	Vector *vector = malloc(sizeof(Vector));
	vector->array = array;
	vector->elem_size = elem_size;
	vector->length = 0;
	vector->capacity = actual_size;

	return vector;
}

/**
 * Clones a vector into new memory.
 *
 * @param old The old vector to copy from
 * @return The new copied vector
 */
Vector* clone(Vector* old) {
	Vector *new = malloc(sizeof(Vector));
	void *array = calloc(old->capacity, old->elem_size);

	new->array = array;
	new->elem_size = old->elem_size;
	new->length = old->length;
	new->capacity = old->capacity;
	memcpy(array, old->array, old->elem_size * old->length);

	return new;
}

/**
 * Gets the element at a specific index of the vector.
 *
 * @param vector The vector
 * @param index The index to retrieve the element from
 * @return The value as void*
 */
void* get_elem(Vector *vector, int index) {
	return (void*) (vector->array + (index * vector->elem_size));
}

/**
 * Sets an element at a particular index to a given value
 *
 * @param vector The vector
 * @param index The index to set the element of
 * @param element The data to set the element to
 */
void set_elem(Vector *vector, int index, void *element) {
	memcpy(vector->array + (index * vector->elem_size), element, vector->elem_size);
}

/**
 * Removes an element from a vector and shifts the rest of the vector across.
 *
 * @param vector The vector
 * @param index The index of the item to remove
 */
void remove_elem(Vector *vector, int index) {
	for (int i = index + 1; i < vector->length; i++) {
		void *next_elem = get_elem(vector, i);
		set_elem(vector, i - 1, next_elem);
	}
	vector->length--;
}

/**
 * Searches a vector and returns the index of where a given element lies.
 *
 * @param vector The vector to search
 * @param element The data to search for
 * @return The index of the found element, or -1 if it does not exist
 */
int index_of(Vector *vector, void *element) {
	for (int i = 0; i < vector->length; i++) {
		void *candidate = get_elem(vector, i);
		if (memcmp(candidate, element, vector->elem_size) == 0) { // memcmp returns 0 if memory is equal
			return i;
		}
	}

	return -1;
}

/**
 * Whether the vector is currently empty.
 *
 * @param vector The vector
 * @return Whether it is empty
 */
BOOL is_empty(Vector *vector) {
	return vector->length == 0;
}

/**
 * Whether any elements are common between two vectors.
 *
 * @param vector1 The first vector
 * @param vector2 The second vector
 * @return Whether any elements are the same
 */
BOOL any_elems_shared(Vector *vector1, Vector *vector2) {
	if (vector1->elem_size != vector2->elem_size) {
		fprintf(stderr, "ERROR: Attempted to compare vectors with inequal element sizes!\n");
		return FALSE;
	}

	for (int i = 0; i < vector1->length; i++) {
		void *elem1 = get_elem(vector1, i);

		for (int j = 0; j < vector2->length; j++) {
			void *elem2 = get_elem(vector2, j);

			if (memcmp(elem1, elem2, vector1->elem_size) == 0) { // memcmp returns 0 if memory is equal
				return TRUE;
			}
		}
	}

	return FALSE;
}

/**
 * Swaps two elements in a vector from given indexes.
 *
 * @param vector The vector to perform the swap in
 * @param index1 The first index
 * @param index2 The second index
 */
void swap_elems(Vector *vector, int index1, int index2) {
	if (index1 == index2) {
		return;
	}

	void *elem1 = get_elem(vector, index1);
	void *elem2 = get_elem(vector, index2);

	void *temp = malloc(vector->elem_size);
	memcpy(temp, elem1, vector->elem_size);

	set_elem(vector, index1, elem2);
	set_elem(vector, index2, temp);

	free(temp);
}

/**
 * Sorts a vector with a comparison function.
 * Uses selection sort (O(n^2)).
 *
 * @param vector The vector to sort
 * @param sort  The function pointer to compare two similar elements
 *              (Return 1 if elem1 > elem2, 0 if elem1 == elem2, -1 if elem1 < elem2)
 */
void sort_vector(Vector *vector, int (*compare)(void *elem1, void *elem2)) {
	for (int i = 0; i < vector->length - 1; i++) {
		int min_index = i;
		void *min = get_elem(vector, i);

		for (int j = i + 1; j < vector->length; j++) {
			void *candidate = get_elem(vector, j);

			int result = compare(min, candidate);

			if (result == 1) {
				min = candidate;
				min_index = j;
			}
		}

		swap_elems(vector, i, min_index);
	}
}

/**
 * Push an element to the back of the vector.
 *
 * @param vector The vector
 * @param element The element to insert
 */
void push_back(Vector *vector, void *element) {
	if (vector->length >= vector->capacity) {
		expand_vector(vector, vector->capacity * 2);
	}
	set_elem(vector, vector->length, element);
	vector->length++;
}

/**
 * Expands a vector to a new capacity (retaining all current data)
 *
 * @param vector The vector
 * @param new_size The size to expand the vector's capacity to
 */
void expand_vector(Vector *vector, size_t new_size) {
	void *array = vector->array;

	size_t new_bytes = vector->elem_size * new_size;

	void *new_array = realloc(array, new_bytes);

	if (new_array == NULL) { // PANIC!
		fprintf(stderr, "ERROR: Vector expansion failed, possibly out of memory? Exiting...\n");
		exit(1);
		return;
	}

	vector->array = new_array;
	vector->capacity = new_size;
}

/**
 * Memory management: Deallocate a vector.
 *
 * @param vector The vector to deallocate
 */
void free_vector(Vector *vector) {
    free(vector->array);
    free(vector);
}
