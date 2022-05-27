// Copyright (c) 2021, Matthew Bentley (mattreecebentley@gmail.com) www.plflib.org

// zLib license (https://www.zlib.net/zlib_license.html):
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// 	claim that you wrote the original software. If you use this software
// 	in a product, an acknowledgement in the product documentation would be
// 	appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// 	misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.


#ifndef PLF_STACK_H
#define PLF_STACK_H


// Compiler-specific defines:

#if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
	#define PLF_FORCE_INLINE __forceinline

	#if _MSC_VER >= 1600
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_STATIC_ASSERT(check, message) static_assert(check, message)
	#else
		#define PLF_STATIC_ASSERT(check, message) assert(check)
	#endif
	#if _MSC_VER >= 1700
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
	#endif
	#if _MSC_VER >= 1800
		#define PLF_VARIADICS_SUPPORT // Variadics, in this context, means both variadic templates and variadic macros are supported
		#define PLF_INITIALIZER_LIST_SUPPORT
	#endif
	#if _MSC_VER >= 1900
		#define PLF_ALIGNMENT_SUPPORT
		#define PLF_NOEXCEPT noexcept
		#define PLF_IS_ALWAYS_EQUAL_SUPPORT
	#else
		#define PLF_NOEXCEPT throw()
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
		#define PLF_CONSTEXPR constexpr
	#else
		#define PLF_CONSTEXPR
	#endif

	#if defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L) && _MSC_VER >= 1929
		#define PLF_CPP20_SUPPORT
	#endif
#elif defined(__cplusplus) && __cplusplus >= 201103L // C++11 support, at least
	#define PLF_FORCE_INLINE // note: GCC and clang create faster code without forcing inline

	#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__) // If compiler is GCC/G++
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4 // 4.2 and below do not support variadic templates
			#define PLF_MOVE_SEMANTICS_SUPPORT
			#define PLF_VARIADICS_SUPPORT
			#define PLF_STATIC_ASSERT(check, message) static_assert(check, message)
		#else
			#define PLF_STATIC_ASSERT(check, message) assert(check)
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4 // 4.3 and below do not support initializer lists
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __GNUC__ > 4
			#define PLF_NOEXCEPT noexcept
		#else
			#define PLF_NOEXCEPT throw()
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#endif
		#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 8) || __GNUC__ > 4
			#define PLF_ALIGNMENT_SUPPORT
		#endif
		#if __GNUC__ >= 5 // GCC v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif
		#if __GNUC__ > 6
			#define PLF_IS_ALWAYS_EQUAL_SUPPORT
		#endif
	#elif defined(__clang__) && !defined(__GLIBCXX__) && !defined(_LIBCPP_CXX03_LANG)
		#if __clang_major__ >= 3 // clang versions < 3 don't support __has_feature() or traits
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_TYPE_TRAITS_SUPPORT

			#if __has_feature(cxx_alignas) && __has_feature(cxx_alignof)
				#define PLF_ALIGNMENT_SUPPORT
			#endif
			#if __has_feature(cxx_noexcept)
				#define PLF_NOEXCEPT noexcept
				#define PLF_IS_ALWAYS_EQUAL_SUPPORT
			#else
				#define PLF_NOEXCEPT throw()
			#endif
			#if __has_feature(cxx_rvalue_references) && !defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES)
				#define PLF_MOVE_SEMANTICS_SUPPORT
			#endif
			#if __has_feature(cxx_static_assert)
				#define PLF_STATIC_ASSERT(check, message) static_assert(check, message)
			#else
				#define PLF_STATIC_ASSERT(check, message) assert(check)
			#endif
			#if __has_feature(cxx_variadic_templates) && !defined(_LIBCPP_HAS_NO_VARIADICS)
				#define PLF_VARIADICS_SUPPORT
			#endif
			#if (__clang_major__ == 3 && __clang_minor__ >= 1) || __clang_major__ > 3
				#define PLF_INITIALIZER_LIST_SUPPORT
			#endif
		#endif
	#elif defined(__GLIBCXX__) // Using another compiler type with libstdc++ - we are assuming full c++11 compliance for compiler - which may not be true
		#if __GLIBCXX__ >= 20080606
			#define PLF_MOVE_SEMANTICS_SUPPORT
			#define PLF_VARIADICS_SUPPORT
			#define PLF_STATIC_ASSERT(check, message) static_assert(check, message)
		#else
			#define PLF_STATIC_ASSERT(check, message) assert(check)
		#endif
		#if __GLIBCXX__ >= 20090421
			#define PLF_INITIALIZER_LIST_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20120322
			#define PLF_ALLOCATOR_TRAITS_SUPPORT
			#define PLF_NOEXCEPT noexcept
		#else
			#define PLF_NOEXCEPT throw()
		#endif
		#if __GLIBCXX__ >= 20130322
			#define PLF_ALIGNMENT_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20150422 // libstdc++ v4.9 and below do not support std::is_trivially_copyable
			#define PLF_TYPE_TRAITS_SUPPORT
		#endif
		#if __GLIBCXX__ >= 20160111
			#define PLF_IS_ALWAYS_EQUAL_SUPPORT
		#endif
	#elif defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) // Special case for checking C++11 support with libCPP
		#define PLF_STATIC_ASSERT(check, message) assert(check)
		#define PLF_NOEXCEPT throw()
		#if !defined(_LIBCPP_HAS_NO_VARIADICS)
			#define PLF_VARIADICS_SUPPORT
		#endif
	#else // Assume type traits and initializer support for other compilers and standard library implementations
		#define PLF_MOVE_SEMANTICS_SUPPORT
		#define PLF_STATIC_ASSERT(check, message) static_assert(check, message)
		#define PLF_VARIADICS_SUPPORT
		#define PLF_TYPE_TRAITS_SUPPORT
		#define PLF_ALLOCATOR_TRAITS_SUPPORT
		#define PLF_ALIGNMENT_SUPPORT
		#define PLF_INITIALIZER_LIST_SUPPORT
		#define PLF_NOEXCEPT noexcept
		#define PLF_IS_ALWAYS_EQUAL_SUPPORT
	#endif

	#if __cplusplus >= 201703L && ((defined(__clang__) && ((__clang_major__ == 3 && __clang_minor__ == 9) || __clang_major__ > 3)) || (defined(__GNUC__) && __GNUC__ >= 7) || (!defined(__clang__) && !defined(__GNUC__))) // assume correct C++17 implementation for non-gcc/clang compilers
		#define PLF_CONSTEXPR constexpr
	#else
		#define PLF_CONSTEXPR
	#endif

	#if __cplusplus > 201704L && ((defined(__clang__) && (__clang_major__ >= 13)) || (defined(__GNUC__) && __GNUC__ >= 10) || (!defined(__clang__) && !defined(__GNUC__)))
		#define PLF_CPP20_SUPPORT
	#endif
#else
	#define PLF_FORCE_INLINE
	#define PLF_STATIC_ASSERT(check, message) assert(check)
	#define PLF_NOEXCEPT throw()
	#define PLF_CONSTEXPR
#endif

#if defined(PLF_IS_ALWAYS_EQUAL_SUPPORT) && defined(PLF_MOVE_SEMANTICS_SUPPORT) && defined(PLF_ALLOCATOR_TRAITS_SUPPORT) && (__cplusplus >= 201703L || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)))
	#define PLF_NOEXCEPT_MOVE_ASSIGN(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_move_assignment::value || std::allocator_traits<the_allocator>::is_always_equal::value)
	#define PLF_NOEXCEPT_SWAP(the_allocator) noexcept(std::allocator_traits<the_allocator>::propagate_on_container_swap::value || std::allocator_traits<the_allocator>::is_always_equal::value)
#else
	#define PLF_NOEXCEPT_MOVE_ASSIGN(the_allocator)
	#define PLF_NOEXCEPT_SWAP(the_allocator)
#endif

#undef PLF_IS_ALWAYS_EQUAL_SUPPORT


#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, ...)	std::allocator_traits<the_allocator>::construct(allocator_instance, location, __VA_ARGS__)
	#else
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, data)	std::allocator_traits<the_allocator>::construct(allocator_instance, location, data)
	#endif

	#define PLF_DESTROY(the_allocator, allocator_instance, location) 				std::allocator_traits<the_allocator>::destroy(allocator_instance, location)
	#define PLF_ALLOCATE(the_allocator, allocator_instance, size, hint) 			std::allocator_traits<the_allocator>::allocate(allocator_instance, size, hint)
	#define PLF_DEALLOCATE(the_allocator, allocator_instance, location, size) 	std::allocator_traits<the_allocator>::deallocate(allocator_instance, location, size)
#else
	#ifdef PLF_VARIADICS_SUPPORT
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, ...)		(allocator_instance).construct(location, __VA_ARGS__)
	#else
		#define PLF_CONSTRUCT(the_allocator, allocator_instance, location, data)	(allocator_instance).construct(location, data)
	#endif

	#define PLF_DESTROY(the_allocator, allocator_instance, location) 				(allocator_instance).destroy(location)
	#define PLF_ALLOCATE(the_allocator, allocator_instance, size, hint)	 		(allocator_instance).allocate(size, hint)
	#define PLF_DEALLOCATE(the_allocator, allocator_instance, location, size)	(allocator_instance).deallocate(location, size)
#endif




#include <cstring> // memset, memcpy
#include <cassert> // assert
#include <limits>  // std::numeric_limits
#include <memory> // std::uninitialized_copy, std::allocator
#include <stdexcept> // std::length_error

#ifdef PLF_CPP20_SUPPORT
	#include <algorithm> // lexicographical_three_way_compare
#endif

#ifdef PLF_MOVE_SEMANTICS_SUPPORT
	#include <utility> // std::move
#endif

#ifdef PLF_TYPE_TRAITS_SUPPORT
	#include <cstddef> // offsetof, used in blank()
	#include <type_traits> // std::is_trivially_destructible
#endif




namespace plf
{

template <class element_type, class element_allocator_type = std::allocator<element_type> > class stack : private element_allocator_type // Empty base class optimisation - inheriting allocator functions
{
public:
	// Standard container typedefs:
	typedef element_type																					value_type;
	typedef element_allocator_type																	allocator_type;

	#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::size_type		size_type;
		typedef element_type &																			reference;
		typedef const element_type &																	const_reference;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 		pointer;
		typedef typename std::allocator_traits<element_allocator_type>::const_pointer	const_pointer;
	#else
		typedef typename element_allocator_type::size_type			size_type;
		typedef typename element_allocator_type::reference			reference;
		typedef typename element_allocator_type::const_reference	const_reference;
		typedef typename element_allocator_type::pointer			pointer;
		typedef typename element_allocator_type::const_pointer	const_pointer;
	#endif

private:
	struct group; // Forward declaration for typedefs below

	#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
		typedef typename std::allocator_traits<element_allocator_type>::template rebind_alloc<group> group_allocator_type;
		typedef typename std::allocator_traits<group_allocator_type>::pointer								group_pointer_type;
		typedef typename std::allocator_traits<element_allocator_type>::pointer 							element_pointer_type;
	#else
		typedef typename element_allocator_type::template rebind<group>::other	group_allocator_type;
		typedef typename group_allocator_type::pointer									group_pointer_type;
		typedef typename element_allocator_type::pointer								element_pointer_type;
	#endif


	struct group : private element_allocator_type
	{
		const element_pointer_type		elements;
		group_pointer_type				next_group, previous_group;
		const element_pointer_type		end; // One-past the back element


		#ifdef PLF_VARIADICS_SUPPORT
			group(const size_type elements_per_group, group_pointer_type const previous = NULL):
				elements(PLF_ALLOCATE(element_allocator_type, *this, elements_per_group, (previous == NULL) ? 0 : previous->elements)),
				next_group(NULL),
				previous_group(previous),
				end(elements + elements_per_group)
			{}


		#else
			// This is a hack around the fact that element_allocator_type::construct only supports copy construction in C++03 and copy elision does not occur on the vast majority of compilers in this circumstance. And to avoid running out of memory (and performance loss) from allocating the same block twice, we're allocating in this constructor and moving data in the copy constructor.
			group(const size_type elements_per_group, group_pointer_type const previous = NULL) PLF_NOEXCEPT:
				elements(NULL),
				next_group(reinterpret_cast<group_pointer_type>(elements_per_group)),
				previous_group(previous),
				end(NULL)
			{}


			// Not a real copy constructor ie. actually a move constructor. Only used for allocator.construct in C++03 for reasons stated above:
			group(const group &source):
				element_allocator_type(source),
				elements(PLF_ALLOCATE(element_allocator_type, *this, reinterpret_cast<size_type>(source.next_group), (source.previous_group == NULL) ? 0 : source.previous_group->elements)),
				next_group(NULL),
				previous_group(source.previous_group),
				end(elements + reinterpret_cast<size_type>(source.next_group))
			{}
		#endif



		~group() PLF_NOEXCEPT
		{
			// Null check not necessary (for empty group and copied group as above) as deallocate will do it's own null check.
			PLF_DEALLOCATE(element_allocator_type, *this, elements, static_cast<size_type>(end - elements));
		}
	};


	group_pointer_type		current_group, first_group; // current group is location of top pointer, first_group is 'front' group, saves performance for ~stack etc
	element_pointer_type		top_element, start_element, end_element; // start_element/end_element cache current_group->end/elements for better performance
	size_type					total_size, total_capacity, min_block_capacity;
	struct ebco_pair : group_allocator_type // Packaging the group allocator with the least-used member variable, for empty-base-class optimization
	{
		size_type max_block_capacity;
		explicit ebco_pair(const size_type max_elements) PLF_NOEXCEPT: max_block_capacity(max_elements) {};
	}								group_allocator_pair;



	inline void check_capacities_conformance(const size_type min, const size_type max) const
	{
  		if (min < 2 || min > max || max > (std::numeric_limits<size_type>::max() / 2))
		{
			throw std::length_error("Supplied memory block capacities outside of allowable ranges");
		}
	}


	#define PLF_MIN_BLOCK_CAPACITY (sizeof(element_type) * 8 > (sizeof(*this) + sizeof(group)) * 2) ? 8 : (((sizeof(*this) + sizeof(group)) * 2) / sizeof(element_type)) + 1


public:

	// Default constructor:
	stack() PLF_NOEXCEPT:
		element_allocator_type(element_allocator_type()),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(PLF_MIN_BLOCK_CAPACITY),
		group_allocator_pair(std::numeric_limits<size_type>::max() / 2)
	{}


	// Allocator-extended constructor:
	explicit stack(const element_allocator_type &alloc):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(PLF_MIN_BLOCK_CAPACITY),
		group_allocator_pair(std::numeric_limits<size_type>::max() / 2)
	{}



	// Constructor with minimum & maximum group size parameters:
	stack(const size_type min, const size_type max = (std::numeric_limits<size_type>::max() / 2)):
		element_allocator_type(element_allocator_type()),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(min),
		group_allocator_pair(max)
	{
		check_capacities_conformance(min, max);
	}



	// Allocator-extended constructor with minimum & maximum group size parameters:
	stack(const size_type min, const size_type max, const element_allocator_type &alloc):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(min),
		group_allocator_pair(max)
	{
		check_capacities_conformance(min, max);
	}



private:

	void allocate_new_group(const size_type capacity, group_pointer_type const previous_group)
	{
		previous_group->next_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, previous_group);

		try
		{
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, previous_group->next_group, capacity, previous_group);
			#else
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, previous_group->next_group, group(capacity, previous_group));
			#endif
		}
		catch (...)
		{
			PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, previous_group->next_group, 1);
			throw;
		}

		total_capacity += capacity;
	}



	inline void deallocate_group(group_pointer_type const the_group) PLF_NOEXCEPT
	{
		PLF_DESTROY(group_allocator_type, group_allocator_pair, the_group);
		PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, the_group, 1);
	}



	void initialize()
	{
		first_group = current_group = PLF_ALLOCATE(group_allocator_type, group_allocator_pair, 1, 0);

		try
		{
			#ifdef PLF_VARIADICS_SUPPORT
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, min_block_capacity);
			#else
				PLF_CONSTRUCT(group_allocator_type, group_allocator_pair, first_group, group(min_block_capacity));
			#endif
		}
		catch (...)
		{
			PLF_DEALLOCATE(group_allocator_type, group_allocator_pair, first_group, 1);
			first_group = current_group = NULL;
			throw;
		}

		start_element = top_element = first_group->elements;
		end_element = first_group->end;
		total_capacity = min_block_capacity;
	}



	inline void progress_to_next_group() // used by push/emplace
	{
		if (current_group->next_group == NULL) // no reserved groups or groups left over from previous pops, allocate new group
		{
			allocate_new_group((total_size < group_allocator_pair.max_block_capacity) ? total_size : group_allocator_pair.max_block_capacity, current_group);
		}

		current_group = current_group->next_group;
		start_element = top_element = current_group->elements;
		end_element = current_group->end;
	}



	void copy_from_source(const stack &source) // Note: this function is only called on an empty un-initialize()'d stack
	{
		assert(&source != this);

		if (source.total_size == 0)
		{
			return;
		}

		group_pointer_type current_copy_group = source.first_group;
		const group_pointer_type end_copy_group = source.current_group;

		if (source.total_size <= group_allocator_pair.max_block_capacity) // most common case
		{
			const size_type original_min_block_capacity = min_block_capacity;
			min_block_capacity = source.total_size;
			initialize();
			min_block_capacity = original_min_block_capacity;

			// Copy groups to this stack:
			while (current_copy_group != end_copy_group)
			{
				std::uninitialized_copy(current_copy_group->elements, current_copy_group->end, top_element);
				top_element += current_copy_group->end - current_copy_group->elements;
				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			std::uninitialized_copy(source.start_element, source.top_element + 1, top_element);
			top_element += source.top_element - source.start_element; // This should make top_element == the last "pushed" element, rather than the one past it
			end_element = top_element + 1; // Since we have created a single group where capacity == size, this is correct
			total_size = source.total_size;
		}
		else // uncommon edge case, so not optimising:
		{
			min_block_capacity = group_allocator_pair.max_block_capacity;

			while (current_copy_group != end_copy_group)
			{
				for (element_pointer_type element_to_copy = current_copy_group->elements; element_to_copy != current_copy_group->end; ++element_to_copy)
				{
					push(*element_to_copy);
				}

				current_copy_group = current_copy_group->next_group;
			}

			// Handle special case of last group:
			for (element_pointer_type element_to_copy = source.start_element; element_to_copy != source.top_element + 1; ++element_to_copy)
			{
				push(*element_to_copy);
			}

			min_block_capacity = source.min_block_capacity;
		}
	}



	void destroy_all_data() PLF_NOEXCEPT
	{
		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR(!std::is_trivially_destructible<element_type>::value) // Avoid iteration for trivially-destructible types eg. POD, structs, classes with empty destructor.
		#endif // If compiler doesn't support traits, iterate regardless - trivial destructors will not be called, hopefully compiler will optimise this loop out for POD types
		{
			if (total_size != 0)
			{
				while (first_group != current_group)
				{
					const element_pointer_type past_end = first_group->end;

					for (element_pointer_type element_pointer = first_group->elements; element_pointer != past_end; ++element_pointer)
					{
						PLF_DESTROY(element_allocator_type, *this, element_pointer);
					}

					const group_pointer_type next_group = first_group->next_group;
					deallocate_group(first_group);
					first_group = next_group;
				}

				// Special case for current group:
				const element_pointer_type past_end = top_element + 1;

				for (element_pointer_type element_pointer = start_element; element_pointer != past_end; ++element_pointer)
				{
					PLF_DESTROY(element_allocator_type, *this, element_pointer);
				}

				first_group = first_group->next_group; // To further process reserved groups in the following loop
				deallocate_group(current_group);
			}
		}

		total_size = 0;

		while (first_group != NULL)
		{
			current_group = first_group;
			first_group = first_group->next_group;
			deallocate_group(current_group);
		}
	}



	inline void blank() PLF_NOEXCEPT
	{
		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value) // if all pointer types are trivial, we can just nuke it from orbit with memset (NULL is always 0 in C++):
			{
				std::memset(static_cast<void *>(this), 0, offsetof(stack, min_block_capacity));
			}
			else
		#endif
		{
			current_group = NULL;
			first_group = NULL;
			top_element = NULL;
			start_element = NULL;
			end_element = NULL;
			total_size = 0;
			total_capacity = 0;
		}
	}



public:

	// Copy constructor:
	stack(const stack &source):
		element_allocator_type(source),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(source.min_block_capacity),
		group_allocator_pair(source.group_allocator_pair.max_block_capacity)
	{
		copy_from_source(source);
	}



	// Allocator-extended copy constructor:
	stack(const stack &source, const allocator_type &alloc):
		element_allocator_type(alloc),
		current_group(NULL),
		first_group(NULL),
		top_element(NULL),
		start_element(NULL),
		end_element(NULL),
		total_size(0),
		total_capacity(0),
		min_block_capacity(source.min_block_capacity),
		group_allocator_pair(source.group_allocator_pair.max_block_capacity)
	{
		copy_from_source(source);
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// move constructor
		stack(stack &&source) PLF_NOEXCEPT:
			element_allocator_type(source),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			top_element(std::move(source.top_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_size(source.total_size),
			total_capacity(source.total_capacity),
			min_block_capacity(source.min_block_capacity),
			group_allocator_pair(source.group_allocator_pair.max_block_capacity)
		{
			source.blank();
		}


		// allocator-extended move constructor
		stack(stack &&source, const allocator_type &alloc):
			element_allocator_type(alloc),
			current_group(std::move(source.current_group)),
			first_group(std::move(source.first_group)),
			top_element(std::move(source.top_element)),
			start_element(std::move(source.start_element)),
			end_element(std::move(source.end_element)),
			total_size(source.total_size),
			total_capacity(source.total_capacity),
			min_block_capacity(source.min_block_capacity),
			group_allocator_pair(source.group_allocator_pair.max_block_capacity)
		{
			source.blank();
		}
	#endif



	~stack() PLF_NOEXCEPT
	{
		destroy_all_data();
	}



	void push(const element_type &element)
	{
		if (top_element == NULL)
		{
			initialize();
		}
		else if (++top_element == end_element) // ie. out of capacity for current element memory block
		{
			progress_to_next_group();
		}

		// Create element:
		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR (std::is_nothrow_copy_constructible<element_type>::value)
			{
				PLF_CONSTRUCT(element_allocator_type, *this, top_element, element);
			}
			else
		#endif
		{
			try
			{
				PLF_CONSTRUCT(element_allocator_type, *this, top_element, element);
			}
			catch (...)
			{
				if (top_element == start_element && current_group != first_group) // for post-initialize push
				{
					current_group = current_group->previous_group;
					start_element = current_group->elements;
					top_element = current_group->end - 1;
				}
				else
				{
					--top_element;
				}

				throw;
			}
		}

		++total_size;
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// Note: the reason for code duplication from non-move push, as opposed to using std::forward for both, was because most compilers didn't actually create as-optimal code in that strategy. Also C++03 compatibility.
		void push(element_type &&element)
		{
			if (top_element == NULL)
			{
				initialize();
			}
			else if (++top_element == end_element)
			{
				progress_to_next_group();
			}


			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
				{
					PLF_CONSTRUCT(element_allocator_type, *this, top_element, std::move(element));
				}
				else
			#endif
			{
				try
				{
					PLF_CONSTRUCT(element_allocator_type, *this, top_element, std::move(element));
				}
				catch (...)
				{
					if (top_element == start_element && current_group != first_group)
					{
						current_group = current_group->previous_group;
						start_element = current_group->elements;
						top_element = current_group->end - 1;
					}
					else
					{
						--top_element;
					}

					throw;
				}
			}

			++total_size;
		}
	#endif




	#ifdef PLF_VARIADICS_SUPPORT
		template<typename... arguments>
		void emplace(arguments &&... parameters)
		{
			if (top_element == NULL)
			{
				initialize();
			}
			else if (++top_element == end_element)
			{
				progress_to_next_group();
			}


			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_nothrow_move_constructible<element_type>::value)
				{
					PLF_CONSTRUCT(element_allocator_type, *this, top_element, std::forward<arguments>(parameters)...);
				}
				else
			#endif
			{
				try
				{
					PLF_CONSTRUCT(element_allocator_type, *this, top_element, std::forward<arguments>(parameters)...);
				}
				catch (...)
				{
					if (top_element == start_element && current_group != first_group)
					{
						current_group = current_group->previous_group;
						start_element = current_group->elements;
						top_element = current_group->end - 1;
					}
					else
					{
						--top_element;
					}

					throw;
				}
			}

			++total_size;
		}
	#endif



	inline PLF_FORCE_INLINE reference top() const // Exception may occur if stack is empty in release mode
	{
		assert(total_size != 0);
		return *top_element;
	}



	void pop() // Exception may occur if stack is empty
	{
		assert(total_size != 0);

		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR (!std::is_trivially_destructible<element_type>::value)
		#endif
		{
			PLF_DESTROY(element_allocator_type, *this, top_element);
		}

		if ((--total_size != 0) & (top_element-- == start_element))
		{ // ie. is start element, but not first group in stack
			current_group = current_group->previous_group;
			start_element = current_group->elements;
			end_element = current_group->end;
			top_element = end_element - 1;
		}
	}



	inline stack & operator = (const stack &source)
	{
		assert(&source != this);

		destroy_all_data();
		stack temp(source);

		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			*this = std::move(temp); // Avoid generating 2nd temporary
		#else
			swap(temp);
		#endif

		return *this;
	}



	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		// Move assignment
		stack & operator = (stack &&source) PLF_NOEXCEPT_MOVE_ASSIGN(allocator_type)
		{
			assert (&source != this);

			destroy_all_data();

			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value)
				{
					std::memcpy(static_cast<void *>(this), &source, sizeof(stack));
				}
				else
			#endif
			{
				current_group = std::move(source.current_group);
				first_group = std::move(source.first_group);
				top_element = std::move(source.top_element);
				start_element = std::move(source.start_element);
				end_element = std::move(source.end_element);
				total_size = source.total_size;
				total_capacity = source.total_capacity;
				min_block_capacity = source.min_block_capacity;
				group_allocator_pair.max_block_capacity = source.group_allocator_pair.max_block_capacity;
			}

			source.blank();
			return *this;
		}
	#endif



	#ifdef PLF_CPP20_SUPPORT
		[[nodiscard]]
	#endif
	inline PLF_FORCE_INLINE bool empty() const PLF_NOEXCEPT
	{
		return total_size == 0;
	}



	inline PLF_FORCE_INLINE size_type size() const PLF_NOEXCEPT
	{
		return total_size;
	}



	inline size_type max_size() const PLF_NOEXCEPT
	{
		#ifdef PLF_ALLOCATOR_TRAITS_SUPPORT
			return std::allocator_traits<element_allocator_type>::max_size(*this);
		#else
			return element_allocator_type::max_size();
		#endif
	}



	size_type capacity() const PLF_NOEXCEPT
	{
		return total_capacity;
	}



	size_type memory() const PLF_NOEXCEPT
	{
		size_type memory_use = sizeof(*this) + (sizeof(value_type) * total_capacity);
		group_pointer_type temp_group = first_group;

		while (temp_group != NULL)
		{
			memory_use += sizeof(group);
			temp_group = temp_group->next_group;
		}

		return memory_use;
	}



private:

	#ifdef PLF_MOVE_SEMANTICS_SUPPORT
		void move_from_source(stack &source) // This function is a mirror copy of copy_from_source, with move instead of copy
		{
			assert(&source != this);

			if (source.total_size == 0)
			{
				return;
			}

			group_pointer_type current_copy_group = source.first_group;
			const group_pointer_type end_copy_group = source.current_group;

			if (source.total_size <= group_allocator_pair.max_block_capacity)
			{
				min_block_capacity = source.total_size; // total_size is set to source size in caller
				initialize();
				min_block_capacity = source.min_block_capacity;

				while (current_copy_group != end_copy_group)
				{
					std::uninitialized_copy(std::make_move_iterator(current_copy_group->elements), std::make_move_iterator(current_copy_group->end), top_element);
					top_element += current_copy_group->end - current_copy_group->elements;
					current_copy_group = current_copy_group->next_group;
				}

				std::uninitialized_copy(std::make_move_iterator(source.start_element), std::make_move_iterator(source.top_element + 1), top_element);
				top_element += source.top_element - source.start_element;
				end_element = top_element + 1;
				total_size = source.total_size;
			}
			else
			{
				min_block_capacity = group_allocator_pair.max_block_capacity;

				while (current_copy_group != end_copy_group)
				{
					for (element_pointer_type element_to_copy = current_copy_group->elements; element_to_copy != current_copy_group->end; ++element_to_copy)
					{
						push(std::move(*element_to_copy));
					}

					current_copy_group = current_copy_group->next_group;
				}

				for (element_pointer_type element_to_copy = source.start_element; element_to_copy != source.top_element + 1; ++element_to_copy)
				{
					push(std::move(*element_to_copy));
				}

				min_block_capacity = source.min_block_capacity;
			}
		}
	#endif



	inline void consolidate()
	{
		#ifdef PLF_MOVE_SEMANTICS_SUPPORT
			stack temp((min_block_capacity > total_size) ? min_block_capacity : ((total_size > group_allocator_pair.max_block_capacity) ? group_allocator_pair.max_block_capacity : total_size), group_allocator_pair.max_block_capacity); // Make first allocated block as large as total_size, where possible

			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_move_assignable<element_type>::value && std::is_move_constructible<element_type>::value)
				{
					temp.move_from_source(*this);
				}
				else
			#endif
			{
				temp.copy_from_source(*this);
			}

			temp.min_block_capacity = min_block_capacity; // reset to correct value for future clear() or erasures
			*this = std::move(temp);
		#else
			stack temp(*this);
			swap(temp);
		#endif
	}



public:


	void reshape(const size_type min, const size_type max)
	{
		check_capacities_conformance(min, max);

		min_block_capacity = min;
		group_allocator_pair.max_block_capacity = max;

		// Need to check all group sizes, because append might append smaller blocks to the end of a larger block:
		for (group_pointer_type current = first_group; current != NULL; current = current->next_group)
		{
			if (static_cast<size_type>(current->end - current->elements) < min || static_cast<size_type>(current->end - current->elements) > max)
			{
				#ifdef PLF_TYPE_TRAITS_SUPPORT // If type is non-copyable/movable, cannot be consolidated, throw exception:
					if PLF_CONSTEXPR (!((std::is_copy_constructible<element_type>::value && std::is_copy_assignable<element_type>::value) || (std::is_move_constructible<element_type>::value && std::is_move_assignable<element_type>::value)))
					{
						throw;
					}
					else
				#endif
				{
					consolidate();
				}

				return;
			}
		}
	}



	void clear() PLF_NOEXCEPT
	{
		destroy_all_data();
		blank();
	}



	bool operator == (const stack &rh) const PLF_NOEXCEPT
	{
		assert (this != &rh);

		if (total_size != rh.total_size)
		{
			return false;
		}

		if (total_size == 0)
		{
			return true;
		}

		group_pointer_type this_group = first_group, rh_group = rh.first_group;
		element_pointer_type this_pointer = first_group->elements, rh_pointer = rh.first_group->elements;

		while(true)
		{
			if (*this_pointer != *rh_pointer)
			{
				return false;
			}

			if (this_pointer == top_element)
			{
				break;
			}

			if (++this_pointer == this_group->end)
			{
				this_group = this_group->next_group;
				this_pointer = this_group->elements;
			}

			if (++rh_pointer == rh_group->end)
			{
				rh_group = rh_group->next_group;
				rh_pointer = rh_group->elements;
			}
		}

		return true;
	}



	inline bool operator != (const stack &rh) const PLF_NOEXCEPT
	{
		return !(*this == rh);
	}



	#ifdef PLF_CPP20_SUPPORT
		friend auto operator <=> (const stack &lh, const stack &rh)
		{
			return std::lexicographical_compare_three_way(lh.begin(), lh.end(), rh.begin(), rh.end());
		}
	#endif



	// Remove trailing stack groups (not removed in general 'pop' usage for performance reasons)
	void trim() PLF_NOEXCEPT
	{
		if (current_group == NULL) // ie. stack is empty
		{
			return;
		}

		group_pointer_type temp_group = current_group->next_group;
		current_group->next_group = NULL; // Set to NULL regardless of whether it is already NULL (avoids branching). Cuts off rest of groups from this group.

		while (temp_group != NULL)
		{
			const group_pointer_type next_group = temp_group->next_group;
			total_capacity -= static_cast<size_type>(temp_group->end - temp_group->elements);
			deallocate_group(temp_group);
			temp_group = next_group;
		}
	}



	void shrink_to_fit()
	{
		if (first_group == NULL || total_size == total_capacity)
		{
			return;
		}
		else if (total_size == 0) // Edge case
		{
			clear();
			return;
		}

		consolidate();
	}



	void reserve(size_type reserve_amount)
	{
		if (reserve_amount == 0 || reserve_amount <= total_capacity)
		{
			return;
		}

		reserve_amount -= total_capacity;

		if (reserve_amount < min_block_capacity)
		{
			reserve_amount = min_block_capacity;
		}
		else if (reserve_amount > max_size())
		{
			reserve_amount = max_size();
		}


		size_type number_of_max_capacity_groups = reserve_amount / group_allocator_pair.max_block_capacity,
					remainder = reserve_amount - (number_of_max_capacity_groups * group_allocator_pair.max_block_capacity);

		if (remainder < min_block_capacity)
		{
			remainder = min_block_capacity;
		}

		if (first_group == NULL) // ie. uninitialized stack
		{
			const size_type original_min_elements = min_block_capacity;

			if (remainder != 0)
			{
				min_block_capacity = remainder;
				remainder = 0;
			}
			else
			{
				min_block_capacity = group_allocator_pair.max_block_capacity;
				--number_of_max_capacity_groups;
			}

			initialize();
			--top_element;
			min_block_capacity = original_min_elements;
		}


		group_pointer_type temp_group = current_group;

		// navigate to end of all current reserved groups (if they exist):
		while (temp_group->next_group != NULL)
		{
			temp_group = temp_group->next_group;
		}


		if (remainder != 0)
		{
			allocate_new_group(remainder, temp_group);
			temp_group = temp_group->next_group;
		}


		while(number_of_max_capacity_groups != 0)
		{
			allocate_new_group(group_allocator_pair.max_block_capacity, temp_group);
			temp_group = temp_group->next_group;
			--number_of_max_capacity_groups;
		}
	}



	inline allocator_type get_allocator() const PLF_NOEXCEPT
	{
		return element_allocator_type();
	}



	void append(stack &source)
	{
		// Process: if there are unused memory spaces at the end of the last current back group of the chain, fill those up
		// with elements from the source groups, starting from the back. Then link the destination stack's groups to the source stack's groups.

		if (source.total_size == 0)
		{
			return;
		}
		else if (total_size == 0)
		{
			#ifdef PLF_MOVE_SEMANTICS_SUPPORT
				*this = std::move(source);
			#else
				destroy_all_data();
				blank();
				swap(source);
			#endif

			return;
		}

		total_size += source.total_size;


		// Fill up the last group in *this with elements from the source:
		size_type elements_to_be_transferred = static_cast<size_type>(end_element - ++top_element);
		size_type current_source_group_size = static_cast<size_type>((source.top_element - source.start_element) + 1);

		while (elements_to_be_transferred >= current_source_group_size)
		{
			// Use the fastest method for moving elements, while preserving values if allocator provides non-trivial pointers:
			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_trivially_copyable<element_type>::value && std::is_trivially_destructible<element_type>::value)
				{
					std::memcpy(static_cast<void *>(&*top_element), static_cast<void *>(&*source.start_element), current_source_group_size * sizeof(element_type));
				}
				#ifdef PLF_MOVE_SEMANTICS_SUPPORT
					else if PLF_CONSTEXPR (std::is_move_constructible<element_type>::value)
					{
						std::uninitialized_copy(std::make_move_iterator(source.start_element), std::make_move_iterator(source.top_element + 1), top_element);
					}
				#endif
				else
			#endif
			{
				std::uninitialized_copy(source.start_element, source.top_element + 1, top_element);

				for (element_pointer_type element_pointer = source.start_element; element_pointer != source.top_element + 1; ++element_pointer)
				{
					PLF_DESTROY(element_allocator_type, source, element_pointer);
				}
			}

			top_element += current_source_group_size;

			if (source.current_group == source.first_group)
			{
				--top_element;
				source.clear();
				return;
			}

			elements_to_be_transferred -= current_source_group_size;
			source.current_group = source.current_group->previous_group;
			source.start_element = source.current_group->elements;
			source.end_element = source.current_group->end;
			source.top_element = source.end_element - 1;
			current_source_group_size = static_cast<size_type>(source.end_element - source.start_element);
		}


		if (elements_to_be_transferred != 0)
		{
			element_pointer_type const start = source.top_element - (elements_to_be_transferred - 1);

			#ifdef PLF_TYPE_TRAITS_SUPPORT
				if PLF_CONSTEXPR (std::is_trivially_copyable<element_type>::value && std::is_trivially_destructible<element_type>::value) // Avoid iteration for trivially-destructible iterators ie. all iterators, unless allocator returns non-trivial pointers
				{
					std::memcpy(static_cast<void *>(&*top_element), static_cast<void *>(&*start), elements_to_be_transferred * sizeof(element_type));
				}
				#ifdef PLF_MOVE_SEMANTICS_SUPPORT
					else if PLF_CONSTEXPR (std::is_move_constructible<element_type>::value)
					{
						std::uninitialized_copy(std::make_move_iterator(start), std::make_move_iterator(source.top_element + 1), top_element);
					}
				#endif
				else
			#endif
			{
				std::uninitialized_copy(start, source.top_element + 1, top_element);

				// The following loop is necessary because the allocator may return non-trivially-destructible pointer types, making iterator a non-trivially-destructible type:
				for (element_pointer_type element_pointer = start; element_pointer != source.top_element + 1; ++element_pointer)
				{
					PLF_DESTROY(element_allocator_type, source, element_pointer);
				}
			}

			source.top_element = start - 1;
		}

		// Trim trailing groups on both, link source and destinations groups and remove references to source groups from source:
		source.trim();
		trim();


		// Throw if incompatible group capacity found:
		if (source.min_block_capacity < min_block_capacity || source.group_allocator_pair.max_block_capacity > group_allocator_pair.max_block_capacity)
		{
			for (group_pointer_type current = source.first_group; current != NULL; current = current->next_group)
			{
				if (static_cast<size_type>(current->end - current->elements) < min_block_capacity || static_cast<size_type>(current->end - current->elements) > group_allocator_pair.max_block_capacity)
				{
					throw std::length_error("A source memory block capacity is outside of the destination's minimum or maximum memory block capacity limits - please change either the source or the destination's min/max block capacity limits using reshape() before calling append() in this case");
				}
			}
		}


		current_group->next_group = source.first_group;
		source.first_group->previous_group = current_group;

		current_group = source.current_group;
		top_element = source.top_element;
		start_element = source.start_element;
		end_element = source.end_element;

		// Correct group sizes if necessary:
		if (source.min_block_capacity < min_block_capacity)
		{
			min_block_capacity = source.min_block_capacity;
		}

		if (source.group_allocator_pair.max_block_capacity < group_allocator_pair.max_block_capacity)
		{
			group_allocator_pair.max_block_capacity = source.group_allocator_pair.max_block_capacity;
		}

		source.blank();
	}



	void swap(stack &source) PLF_NOEXCEPT_SWAP(element_allocator_type)
	{
		#ifdef PLF_TYPE_TRAITS_SUPPORT
			if PLF_CONSTEXPR (std::is_trivial<group_pointer_type>::value && std::is_trivial<element_pointer_type>::value) // if all pointer types are trivial we can just copy using memcpy - faster
			{
				char temp[sizeof(stack)];
				std::memcpy(&temp, static_cast<void *>(this), sizeof(stack));
				std::memcpy(static_cast<void *>(this), static_cast<void *>(&source), sizeof(stack));
				std::memcpy(static_cast<void *>(&source), &temp, sizeof(stack));
			}
			#ifdef PLF_MOVE_SEMANTICS_SUPPORT // If pointer types are not trivial, moving them is probably going to be more efficient than copying them below
				else if PLF_CONSTEXPR (std::is_move_assignable<group_pointer_type>::value && std::is_move_assignable<element_pointer_type>::value && std::is_move_constructible<group_pointer_type>::value && std::is_move_constructible<element_pointer_type>::value)
				{
					stack temp(std::move(source));
					source = std::move(*this);
					*this = std::move(temp);
				}
			#endif
			else
		#endif
		{
			const group_pointer_type	swap_current_group = current_group, swap_first_group = first_group;
			const element_pointer_type	swap_top_element = top_element, swap_start_element = start_element, swap_end_element = end_element;
			const size_type				swap_total_size = total_size, swap_total_capacity = total_capacity, swap_min_block_capacity = min_block_capacity, swap_max_block_capacity = group_allocator_pair.max_block_capacity;

			current_group = source.current_group;
			first_group = source.first_group;
			top_element = source.top_element;
			start_element = source.start_element;
			end_element = source.end_element;
			total_size = source.total_size;
			total_capacity = source.total_capacity;
			min_block_capacity = source.min_block_capacity;
			group_allocator_pair.max_block_capacity = source.group_allocator_pair.max_block_capacity;

			source.current_group = swap_current_group;
			source.first_group = swap_first_group;
			source.top_element = swap_top_element;
			source.start_element = swap_start_element;
			source.end_element = swap_end_element;
			source.total_size = swap_total_size;
			source.total_capacity = swap_total_capacity;
			source.min_block_capacity = swap_min_block_capacity;
			source.group_allocator_pair.max_block_capacity = swap_max_block_capacity;
		}
	}


}; // stack

} // plf namespace


namespace std
{

template <class element_type, class element_allocator_type>
inline void swap (plf::stack<element_type, element_allocator_type> &a, plf::stack<element_type, element_allocator_type> &b) PLF_NOEXCEPT_SWAP(element_allocator_type)
{
	a.swap(b);
}

}


#undef PLF_MIN_BLOCK_CAPACITY
#undef PLF_FORCE_INLINE
#undef PLF_ALIGNMENT_SUPPORT
#undef PLF_INITIALIZER_LIST_SUPPORT
#undef PLF_TYPE_TRAITS_SUPPORT
#undef PLF_ALLOCATOR_TRAITS_SUPPORT
#undef PLF_VARIADICS_SUPPORT
#undef PLF_MOVE_SEMANTICS_SUPPORT
#undef PLF_NOEXCEPT
#undef PLF_NOEXCEPT_SWAP
#undef PLF_NOEXCEPT_MOVE_ASSIGN
#undef PLF_CONSTEXPR
#undef PLF_CPP20_SUPPORT
#undef PLF_STATIC_ASSERT

#undef PLF_CONSTRUCT
#undef PLF_DESTROY
#undef PLF_ALLOCATE
#undef PLF_DEALLOCATE

#endif // PLF_STACK_H
