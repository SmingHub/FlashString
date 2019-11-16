/**
 * Object.hpp - Definitions and macros common to all object types
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

#include "config.hpp"
#include "Utility.hpp"
#include "ObjectIterator.hpp"

/**
 * @brief Wrap a type declaration so it can be passed with commas in it
 * @note Example:
 *
 *	template <typename ElementType, size_t Columns>
 *	struct MultiRow
 *	{
 *		ElementType values[Columns];
 *	}
 *
 * These fail:
 *
 * 	DECLARE_FSTR_ARRAY(myArray, MultiRow<double, 3>);
 * 	DECLARE_FSTR_ARRAY(myArray, (MultiRow<double, 3>));
 *
 * Use DECL like this:
 *
 * 	DECLARE_FSTR_ARRAY(myArray, DECL((MultiRow<double, 3>)) );
 *
 * Although for this example we should probably do this:
 *
 * 	using MultiRow_double_3 = MultiRow<double, 3>;
 * 	DECLARE_FSTR_ARRAY(myArray, MultiRow_double_3);
 *
 */
#define DECL(t) argument_type<void(t)>::type
template <typename T> struct argument_type;
template <typename T, typename U> struct argument_type<T(U)> {
	typedef U type;
};

/**
 * @brief Define a reference to an object
 * @param name Name for reference
 * @param ObjectType Fully qualified typename of object required, e.g. FSTR::String, FlashString, FSTR::Vector<int>, etc.
 * @param object FSTR::Object instance to cast
 */
#define DEFINE_FSTR_REF(name, ObjectType, object) constexpr const ObjectType& name PROGMEM = object.as<ObjectType>();
#define DEFINE_FSTR_REF_LOCAL(name, ObjectType, object) static DEFINE_FSTR_REF(name, ObjectType, object)

#define DEFINE_FSTR_REF_NAMED(name, ObjectType) DEFINE_FSTR_REF(name, ObjectType, FSTR_DATA_NAME(name).object);

/**
 * @brief Provide internal name for generated flash string structures
 */
#define FSTR_DATA_NAME(name) fstr_data_##name

/**
 * @brief Check structure is POD-compliant and correctly aligned
 */
#define FSTR_CHECK_STRUCT(name)                                                                                        \
	static_assert(std::is_pod<decltype(name)>::value, "FSTR structure not POD");                                       \
	static_assert(offsetof(decltype(name), data) == sizeof(uint32_t), "FSTR structure alignment error");

/**
 * @brief Link the contents of a file
 * @note We need inline assembler's `.incbin` instruction to actually import the data.
 * We use a macro STR() so that if required the name can be resolved from a #defined value.
 * @note This provides a more efficient way to read constant (read-only) file data.
 * The file content is bound into firmware image at link time.
 * @note The String object must be referenced or the linker won't emit it.
 * @note Use PROJECT_DIR to locate files in your project's source tree:
 *  		IMPORT_FSTR_DATA(myFlashData, PROJECT_DIR "/files/my_flash_file.txt");
 * Use COMPONENT_PATH within a component.
 * @note No C/C++ symbol is declared, this is type-dependent and must be done separately:
 * 			extern "C" FlashString myFlashData;
 */
#define STR(x) XSTR(x)
#define XSTR(x) #x
#ifdef __WIN32
#define IMPORT_FSTR_DATA(name, file)                                                                                   \
	__asm__(".section .rodata\n"                                                                                       \
			".global _" STR(name) "\n"                                                                                 \
			".def _" STR(name) "; .scl 2; .type 32; .endef\n"                                                          \
			".align 4\n"                                                                                               \
			"_" STR(name) ":\n"                                                                                        \
			".long _" STR(name) "_end - _" STR(name) " - 4\n"                                                          \
			".incbin \"" file "\"\n"                                                                                   \
			"_" STR(name) "_end:\n");
#else
#ifdef ARCH_HOST
#define IROM_SECTION ".rodata"
#else
#define IROM_SECTION ".irom0.text"
#endif
#define IMPORT_FSTR_DATA(name, file)                                                                                   \
	__asm__(".section " IROM_SECTION "\n"                                                                              \
			".global " STR(name) "\n"                                                                                  \
			".type " STR(name) ", @object\n"                                                                           \
			".align 4\n" STR(name) ":\n"                                                                               \
			".long _" STR(name) "_end - " STR(name) " - 4\n"                                                           \
			".incbin \"" file "\"\n"                                                                                   \
			"_" STR(name) "_end:\n");
#endif

namespace FSTR
{
class ObjectBase
{
public:
	/**
	 * @brief Get the length of the object in elements
	 */
	FORCE_INLINE uint32_t length() const
	{
		return objectPtr()->flashLength;
	}

	/**
	 * @brief Get the object data size in bytes
	 * @note Always an integer multiple of 4 bytes
	 */
	FORCE_INLINE uint32_t size() const
	{
		return ALIGNUP(objectPtr()->flashLength);
	}

	template <class ObjectType> FORCE_INLINE constexpr const ObjectType& as() const
	{
		return *static_cast<const ObjectType*>(this);
	}

	/**
	 * @brief Get a pointer to the flash data
	 */
	FORCE_INLINE const uint8_t* data() const
	{
		return reinterpret_cast<const uint8_t*>(&objectPtr()->flashLength + 1);
	}

	/**
	 * @brief Read contents of a String into RAM
	 * @param offset Zero-based offset from start of flash data to start reading
	 * @param buffer Where to store data
	 * @param count How many bytes to read
	 * @retval size_t Number of bytes actually read
	 */
	size_t read(size_t offset, void* buffer, size_t count) const
	{
		if(offset >= flashLength) {
			return 0;
		}

		count = std::min(flashLength - offset, count);
		memcpy_P(buffer, data() + offset, count);
		return count;
	}

	/**
	 * @brief Read contents of a String into RAM, using flashread()
	 * @param offset Zero-based offset from start of flash data to start reading
	 * @param buffer Where to store data
	 * @param count How many bytes to read
	 * @retval size_t Number of bytes actually read
	 * @note PROGMEM data is accessed via the CPU data cache, so to avoid degrading performance
	 * you can use this method to read data directly from flash memory.
	 * This is appropriate for infrequently accessed data, especially if it is large.
	 * For example, if storing content using `IMPORT_FSTR` instead of SPIFFS then it
	 * is generally better to avoid contaminating the cache.
	 * @see See also `FlashMemoryStream` class.
	 */
	size_t readFlash(size_t offset, void* buffer, size_t count) const;

	FORCE_INLINE bool isCopy() const
	{
		return flashLength & copyBit;
	}

	/* Private member data */

	uint32_t flashLength;
	// const uint8_t data[]

protected:
	static const ObjectBase empty_;
	static constexpr uint32_t copyBit = 0x80000000U;

private:
	FORCE_INLINE const ObjectBase* objectPtr() const
	{
		if(isCopy()) {
			return reinterpret_cast<const ObjectBase*>(flashLength & ~copyBit);
		} else {
			return this;
		}
	}
};

template <class ObjectType, typename ElementType> class Object : public ObjectBase
{
public:
	using Iterator = ObjectIterator<ObjectType, ElementType>;

	/*
	 * @brief Copy constructor
	 * @note FlashStrings are usually passed around by reference or as a pointer,
	 * but for better compatibility with other string types (e.g. String) we
	 * need a working copy constructor.
	 *
	 * As far as the compiler is concerned, a FlashString is only 4 bytes containing
	 * flashLength. The copy constructor stores a pointer to the real FlashString
	 * object here and sets the top bit.
	 */
	Object(const Object& obj)
	{
		if(obj.isCopy()) {
			// Copy of a copy
			flashLength = obj.flashLength;
		} else {
			// Copy of a real Object
			flashLength = reinterpret_cast<uint32_t>(&obj) | copyBit;
		}
	}

	Iterator begin() const
	{
		return Iterator(this->as<ObjectType>(), 0);
	}

	Iterator end() const
	{
		return Iterator(this->as<ObjectType>(), this->length());
	}

	static const ObjectType& empty()
	{
		return empty_.as<ObjectType>();
	}

	/**
	 * @brief Get the length of the array in elements
	 */
	FORCE_INLINE uint32_t length() const
	{
		return ObjectBase::length() / sizeof(ElementType);
	}

	FORCE_INLINE ElementType valueAt(unsigned index) const
	{
		if(index < this->length()) {
			return readValue(this->data() + index);
		} else {
			return ElementType{0};
		}
	}

	/**
	 * @brief Array operator[]
	 */
	FORCE_INLINE ElementType operator[](unsigned index) const
	{
		return this->valueAt(index);
	}

	FORCE_INLINE size_t elementSize() const
	{
		return sizeof(ElementType);
	}

	FORCE_INLINE const ElementType* data() const
	{
		return reinterpret_cast<const ElementType*>(ObjectBase::data());
	}

	/**
	 * @brief Read contents of an Array into RAM
	 * @param index First element to read
	 * @param buffer Where to store data
	 * @param count How many elements to read
	 * @retval size_t Number of elements actually read
	 */
	size_t read(size_t index, ElementType* buffer, size_t count) const
	{
		auto offset = index * sizeof(ElementType);
		count *= sizeof(ElementType);
		return ObjectBase::read(offset, buffer, count) / sizeof(ElementType);
	}

	/**
	 * @brief Read contents of an Array into RAM, using flashread()
	 * @param index First element to read
	 * @param buffer Where to store data
	 * @param count How many elements to read
	 * @retval size_t Number of elements actually read
	 */
	size_t readFlash(size_t index, ElementType* buffer, size_t count) const
	{
		auto offset = index * sizeof(ElementType);
		count *= sizeof(ElementType);
		return ObjectBase::readFlash(offset, buffer, count) / sizeof(ElementType);
	}
};

}; // namespace FSTR
