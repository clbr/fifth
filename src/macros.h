/*
Copyright (C) 2014 Lauri Kasanen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MACROS_H
#define MACROS_H

#if __GNUC__ >= 4

	#define PURE_FUNC __attribute__ ((pure))
	#define NORETURN_FUNC __attribute__ ((noreturn))
	#define CONST_FUNC __attribute__ ((const))
	#define WUR_FUNC __attribute__ ((warn_unused_result))
	#define MALLOC_FUNC __attribute__ ((malloc))

#if __GNUC_MINOR__ >= 3 // GCC 4.3
	#define ALLOC_SIZE1(a) __attribute__ ((alloc_size(a)))
	#define ALLOC_SIZE2(a, b) __attribute__ ((alloc_size(a, b)))
#else
	#define ALLOC_SIZE1(a)
	#define ALLOC_SIZE2(a, b)
#endif

#else // GNUC

	#define PURE_FUNC
	#define NORETURN_FUNC
	#define CONST_FUNC
	#define WUR_FUNC
	#define MALLOC_FUNC
	#define ALLOC_SIZE1(a)
	#define ALLOC_SIZE2(a, b)

#endif // GNUC

#endif
