Associative Maps
================

.. highlight:: C++

Introduction
------------

A ``Map`` is analogous to the Wiring HashMap class, allowing content to be indexed using
a key value.

The Map contains an array of ``MapPair`` structures::

   struct MapPair<KeyType, ContentType> {
      KeyType key_;
      ContentType* content_;
   };

``KeyType`` can be any simple type such as ``char``, ``int``, ``float``, ``enum`` etc.
It may also be a ``String`` Object (or, more precisely, ``String*``).

``ContentType`` can be any Object type (String, Array, Vector or Map).
This allows hierarchical structures to be created.

Example: int => String
----------------------

Here's a basic example using integer keys::

   #include <FlashString/Map.hpp>

   IMPORT_FSTR(content1, PROJECT_DIR "/files/index.html");
   IMPORT_FSTR(content2, PROJECT_DIR "/files/favicon.html");

   DEFINE_FSTR_MAP(intmap, int, FSTR::String,
      {35, &content1},
      {180, &content2}
   );

We can now do this::

   void printValue(int key)
   {
      auto value = intmap[key];
      if(value) {
         Serial.printf("Found '%u' in map, containing %u chars\n", value.key(), value.content().length());
         Serial.println(value.printer());
      } else {
         Serial.printf("Couldn't find '%u' in map\n", key);
      }
   }


Example: String => String
-------------------------

Both the key and the content are stored as Strings::

   #include <FlashString/Map.hpp>

   DEFINE_FSTR_LOCAL(key1, "index.html");
   DEFINE_FSTR_LOCAL(key2, "favicon.ico");
   IMPORT_FSTR(content1, PROJECT_DIR "/files/index.html");
   IMPORT_FSTR(content2, PROJECT_DIR "/files/favicon.html");

   DEFINE_FSTR_MAP(fileMap, FlashString, FlashString,
      {&key1, &content1},
      {&key2, &content2},
   );

We can now do this::

   void onFile(HttpRequest& request, HttpResponse& response)
   {
      String fileName = request.uri.getRelativePath();
      auto& value = fileMap[fileName];
      if(value) {
         // Found
         Serial.printf("Found '%s' in fileMap\n", String(value.key()).c_str());
         auto stream = new FlashMemoryStream(value);
         response.sendDataStream(stream, ContentType::fromFullFileName(fileName));
      } else {
         Serial.printf("File '%s' not found\n", fileName.c_str());
      }
   }

.. note::

   As with ``Vector<String>``, ``Map<String, ...>`` lookups are by default case-insensitive.
   
   If you require a case-sensitive lookup, use the ``indexOf`` method with ``ignoreCase = false``.


Structure
---------

The macro in the first example above produces a structure like this::

   constexpr const struct {
      ObjectBase object;
      MapPair<int, String> data[2];
   } fstr_data_intmap PROGMEM = {
      {16},
      {35, &content1},
      {180, &content2},
   };
   const Map<int, String>& intmap = fstr_data_intmap.object.as<Map<int, String>>();

Note: ``FSTR::`` namespace qualifier omitted for clarity.

Usually, each MapPair is 8 bytes, but if the key is a double or int64 it would be 12 bytes.


Additional Macros
-----------------

DEFINE_FSTR_MAP_DATA(name, KeyType, ContentType, ...)
   Define the map structure without an associated reference.

