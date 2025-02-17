/**
 * Vector.hpp - Defines the Vector class template and associated macros
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
 * @brief Declare a global Vector& reference
 * @param name
 * @param ObjectType
 * @note Use `DEFINE_VECTOR` to instantiate the global Object
 */
#define DECLARE_FSTR_VECTOR(name, ObjectType) extern const FSTR::Vector<ObjectType>& name;

/**
 * @brief Define a Vector Object with global reference
 * @param name Name of Vector& reference to define
 * @param ObjectType
 * @param ... List of ObjectType* pointers
 * @note Size will be calculated
 */
#define DEFINE_FSTR_VECTOR(name, ObjectType, ...)                                                                      \
	static DEFINE_FSTR_VECTOR_DATA(FSTR_DATA_NAME(name), ObjectType, __VA_ARGS__);                                     \
	DEFINE_FSTR_REF_NAMED(name, FSTR::Vector<ObjectType>);

/**
 * @brief Define a Vector Object with local reference
 * @param name Name of Vector& reference to define
 * @param ObjectType
 * @param ... List of ObjectType* pointers
 * @note Size will be calculated
 */
#define DEFINE_FSTR_VECTOR_LOCAL(name, ObjectType, ...)                                                                \
	static DEFINE_FSTR_VECTOR_DATA(FSTR_DATA_NAME(name), ObjectType, __VA_ARGS__);                                     \
	static constexpr DEFINE_FSTR_REF_NAMED(name, FSTR::Vector<ObjectType>);

/**
 * @brief Define a Vector Object with global reference, specifying the number of elements
 * @param name Name of Vector& reference to define
 * @param ObjectType
 * @param size Number of elements
 * @param ... List of ObjectType* pointers
 * @note Use in situations where the array size cannot be automatically calculated
 */
#define DEFINE_FSTR_VECTOR_SIZED(name, ObjectType, size, ...)                                                          \
	static DEFINE_FSTR_VECTOR_DATA_SIZED(FSTR_DATA_NAME(name), ObjectType, size, __VA_ARGS__);                         \
	DEFINE_FSTR_REF_NAMED(name, FSTR::Vector<ObjectType>);

/**
 * @brief Define a Vector Object with local reference, specifying the number of elements
 * @param name Name of Vector& reference to define
 * @param ObjectType
 * @param size Number of elements
 * @param ... List of ObjectType* pointers
 * @note Use in situations where the array size cannot be automatically calculated
 */
#define DEFINE_FSTR_VECTOR_SIZED_LOCAL(name, ObjectType, size, ...)                                                    \
	static DEFINE_FSTR_VECTOR_DATA_SIZED(FSTR_DATA_NAME(name), ObjectType, size, __VA_ARGS__);                         \
	static constexpr DEFINE_FSTR_REF_NAMED(name, FSTR::Vector<ObjectType>);

/**
 * @brief Define a Vector data structure
 * @param name Name of data structure
 * @param ObjectType
 * @param ... List of ObjectType* pointers
 * @note Size will be calculated
 */
#define DEFINE_FSTR_VECTOR_DATA(name, ObjectType, ...)                                                                 \
	DEFINE_FSTR_VECTOR_DATA_SIZED(name, ObjectType, sizeof((const void* []){__VA_ARGS__}) / sizeof(void*), __VA_ARGS__)

/**
 * @brief Define a Vector data structure and specify the number of elements
 * @param name Name of data structure
 * @param ObjectType
 * @param size Number of elements
 * @param ... List of ObjectType* pointers
 * @note Use in situations where the array size cannot be automatically calculated
 */
#define DEFINE_FSTR_VECTOR_DATA_SIZED(name, ObjectType, size, ...)                                                     \
	constexpr const struct {                                                                                           \
		FSTR::ObjectBase object;                                                                                       \
		const ObjectType* data[size];                                                                                  \
	} name PROGMEM = {{sizeof(name.data)}, __VA_ARGS__};                                                               \
	FSTR_CHECK_STRUCT(name);

namespace FSTR
{
/**
 * @brief Class to access a Vector of objects stored in flash
 */
template <class ObjectType> class Vector : public Object<Vector<ObjectType>, ObjectType*>
{
public:
	template <typename ValueType, typename T = ObjectType>
	typename std::enable_if<std::is_same<T, String>::value, int>::type indexOf(const ValueType& value,
																			   bool ignoreCase = true) const
	{
		if(!ignoreCase) {
			return Object<Vector<String>, String*>::indexOf(value);
		}

		auto len = this->length();
		for(unsigned i = 0; i < len; ++i) {
			if(valueAt(i).equalsIgnoreCase(value)) {
				return i;
			}
		}

		return -1;
	}

	const ObjectType& valueAt(unsigned index) const
	{
		if(index < this->length()) {
			auto ptr = this->data()[index];
			if(ptr != nullptr) {
				return *ptr;
			}
		}

		return ObjectType::empty();
	}

	const ObjectType& operator[](unsigned index) const
	{
		return valueAt(index);
	}

	/* Arduino Print support */

	ArrayPrinter<Vector> printer(const WString& separator = ", ") const
	{
		return ArrayPrinter<Vector>(*this, separator);
	}

	size_t printTo(Print& p) const
	{
		return printer().printTo(p);
	}
};

} // namespace FSTR
