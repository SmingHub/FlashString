/**
 * Array.hpp - Defines the Array class and associated macros
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the FlashString Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include "Object.hpp"
#include "ArrayPrinter.hpp"

/**
 * @brief Declare a global Array& reference
 * @param name
 * @param ElementType
 * @note Use `DEFINE_FSTR_ARRAY` to instantiate the global Object
 */
#define DECLARE_FSTR_ARRAY(name, ElementType) extern const FSTR::Array<ElementType>& name;

/**
 * @brief Define an Array Object with global reference
 * @param name Name of Array& reference to define
 * @param ElementType
 * @param ... List of ElementType items
 * @note Unlike String, array is not NUL-terminated
 */
#define DEFINE_FSTR_ARRAY(name, ElementType, ...)                                                                      \
	static DEFINE_FSTR_ARRAY_DATA(FSTR_DATA_NAME(name), ElementType, __VA_ARGS__);                                     \
	DEFINE_FSTR_REF_NAMED(name, FSTR::Array<ElementType>);

/**
 * @brief Define an Array Object with local reference
 * @param name Name of Array& reference to define
 * @param ElementType
 * @param ... List of ElementType items
 */
#define DEFINE_FSTR_ARRAY_LOCAL(name, ElementType, ...)                                                                \
	static DEFINE_FSTR_ARRAY_DATA(FSTR_DATA_NAME(name), ElementType, __VA_ARGS__);                                     \
	static constexpr DEFINE_FSTR_REF_NAMED(name, FSTR::Array<ElementType>);

/**
 * @brief Define an Array data structure
 * @param name Name of data structure
 * @param ElementType
 * @param ... List of ElementType items
 */
#define DEFINE_FSTR_ARRAY_DATA(name, ElementType, ...)                                                                 \
	constexpr const struct {                                                                                           \
		FSTR::ObjectBase object;                                                                                       \
		ElementType data[sizeof((const ElementType[]){__VA_ARGS__}) / sizeof(ElementType)];                            \
	} FSTR_PACKED name PROGMEM = {{sizeof(name.data)}, {__VA_ARGS__}};                                                 \
	FSTR_CHECK_STRUCT(name);

/**
 * @brief Load an Array object into a named local (stack) buffer
 * @note Example:
 *
 * 	DEFINE_FSTR_ARRAY(fsArray, double, 5.33, PI)
 * 	...
 * 	LOAD_FSTR_ARRAY(arr, fsArray)
 * 	printf("arr[0] = %f, %u elements, buffer is %u bytes\n", arr[0], fsArray.length(), sizeof(arr));
 */
#define LOAD_FSTR_ARRAY(name, array)                                                                                   \
	decltype(array)[0] name[(array).size()] __attribute__((aligned(4)));                                               \
	memcpy_aligned(name, (array).data(), (array).size());

/**
 * @brief Define an Array and load it into a named buffer on the stack
 * @note Equivalent to `ElementType name[] = {a, b, c}` except the buffer is word-aligned
 */
#define FSTR_ARRAY_ARRAY(name, ElementType, ...)                                                                       \
	static DEFINE_FSTR_ARRAY_DATA(FSTR_DATA_NAME(name), ElementType, __VA_ARGS__);                                     \
	LOAD_FSTR_ARRAY(name, FSTR_DATA_NAME(name).object)

/**
 * @brief Define an Array containing data from an external file
 * @param name Name for the object
 * @param file Absolute path to the file containing the content
 */
#define IMPORT_FSTR_ARRAY(name, ElementType, file)                                                                     \
	IMPORT_FSTR_DATA(name, file)                                                                                       \
	extern "C" const FSTR::Array<ElementType> name;

namespace FSTR
{
/**
 * @brief Class to access an array of integral values stored in flash
 */
template <typename ElementType> class Array : public Object<Array<ElementType>, ElementType>
{
public:
	/* Arduino Print support */

	/**
	 * @brief Returns a printer object for this array
	 * @note ElementType must be supported by Print
	 */
	ArrayPrinter<Array> printer(const WString& separator = ", ") const
	{
		return ArrayPrinter<Array>(*this, separator);
	}

	size_t printTo(Print& p) const
	{
		return printer().printTo(p);
	}
};

} // namespace FSTR
