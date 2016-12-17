

//@brief Macro expansion into a message struct that can be serialized to/from mqtt strings
//@details In order to use this struct you must define either STRUCT_FIELDS with a 
//X expansion notation of X(type,name) which will expand to type name.  Similiarly
//for the STRUCT_ARRAY_FIELDS this expands into an array of the given type with the
//same X expansion notation of X(type,name,size) which will expand to type name[size].
//@todo currently the STRUCT_ARRAY_FIELDS only use char arrays and convert to and from 
//c strings.  Add more ability to add an array that can be serialized to/from mqtt strings.
struct STRUCT_NAME{
#ifdef STRUCT_FIELDS
	#define X(type, name) type name;
		 STRUCT_FIELDS
	#undef X
#endif
#ifdef STRUCT_ARRAY_FIELDS
	#define X(type, name, size) type name[size];
		STRUCT_ARRAY_FIELDS
	#undef X
#endif

#ifndef SMALL_MEMORY
			 
	int to_mqtt_string(char* buffer, const size_t size)
	{
		if(size < 0 || buffer == nullptr) {
			return -1;	
		}
		
		int offset = 0;
		size_t i = 0;
		//If struct fields are defined then convert the token seperated
		//mqtt string into a single variable. @ref mqtt_helper.hpp for more
		//information on the conversion is done using the mqtt '/' token.
#ifdef STRUCT_FIELDS
	#define X(type, name ) \
	    i = to_mqtt_string_helper(buffer + offset, size - offset, name); \
		offset += i; \
		if(i < 0 || i > size) { return -1; }
		STRUCT_FIELDS
	#undef X
#endif
		//If struct array fields are defined then convert the token seperated
		//mqtt string into a single variable. @ref mqtt_helper.hpp for more
		//information on the conversion is done using the mqtt '/' token.
		//@todo this is expecting c strings only.
#ifdef STRUCT_ARRAY_FIELDS
	#define X(type, name,  size) \
		i = to_mqtt_string_helper(buffer + offset, size - offset,  name); \
		offset += i; \
		if(i < 0 || offset > size) { return -1; }
		STRUCT_ARRAY_FIELDS
	#undef X
#endif
		return offset;
	}
	

	void from_mqtt_string(char* buffer)
	{
		char* pstr = strtok((char*)buffer,"/");
		
		if(pstr != NULL) {
		//@brief If struct fields are defined then convert the values saved in struct 
		//into a mqtt string
		#ifdef STRUCT_FIELDS
			#define X(type, name) \
				type temp_##name; \
				mqtt_string_helper(pstr, temp_##name); \
				name = temp_##name; \
				pstr = strtok(NULL,"/"); \
				if(pstr == NULL) { return; }
				STRUCT_FIELDS
			#undef X
		#endif
		//@brief If struct  array fields are defined then convert the values saved in struct 
		//into a mqtt string
		#ifdef STRUCT_ARRAY_FIELDS
			#define X(type, name, size) \
				mqtt_string_helper(pstr, name); \
				pstr = strtok(NULL,"/"); \
				if(pstr == NULL) { return; }
				STRUCT_ARRAY_FIELDS
			#undef X
		#endif
		}
	}
#endif //SMALL_MEMORY
}__attribute__((packed));



#undef STRUCT_FIELDS
#undef STRUCT_NAME
#undef STRUCT_ARRAY_FIELDS

