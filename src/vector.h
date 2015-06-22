#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

#define vector_struct(T) \
struct \
{ \
	T * data; \
	size_t size; \
	size_t end; \
}

#define vector_init(vec) \
	(vec)->data = 0; \
	(vec)->size = 0; \
	(vec)->end = 0;

#define vector_deinit(vec) \
	free((vec)->data); \
	(vec)->data = 0; \
	(vec)->size = 0; \
	(vec)->end = 0;

#define vector_push(vec, val) \
	if((vec)->end == (vec)->size) \
	{ \
		size_t newsize = (vec)->size ? (vec)->size * 2 : 32; \
		(vec)->data = realloc((vec)->data, sizeof(*(vec)->data) * newsize); \
		(vec)->size = newsize; \
	} \
 \
	(vec)->data[(vec)->end] = (val); \
	(vec)->end ++;

#define vector_pop(vec) \
	if((vec)->end > 0) \
	{ \
		(vec)->end --; \
 \
		if((vec)->end < (vec)->size / 2) \
		{ \
			size_t newsize = (vec)->size / 2; \
			(vec)->data = realloc((vec)->data, sizeof(*(vec)->data) * newsize); \
			(vec)->size = newsize; \
		} \
	}

#define vector_empty(vector) ((vector)->end == 0)

#define vector_back(vector) ((vector)->data[(vector)->end - 1])
#define vector_front(vector) ((vector)->data[0])

#define vector_get(vector, idx) ((vector)->data[idx])
#define vector_getd(vector, idx, def) (((idx) < (vector)->end) ? (vector)->data[(idx)] : (def))
#define vector_idxok(vector, idx) ((idx) < (vector)->end && (idx) >= 0)

#define vector_size(vector) ((vector)->end)

#define vector_foreach(vec, it) \
	for(struct { size_t index; __typeof__(*(vec)->data) * ptr; } it = { 0, &(vec)->data[0] } ; \
		it.index < (vec)->end ; \
		it.index ++ , it.ptr = &(vec)->data[it.index] )

#endif
