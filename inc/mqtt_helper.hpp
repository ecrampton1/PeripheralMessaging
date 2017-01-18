#ifndef MQTT_HELPER_HPP_
#define MQTT_HELPER_HPP_
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

inline int to_mqtt_string_helper(char* buffer, const size_t size, bool val)
{
	return snprintf(buffer, size, "%d", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, int8_t val)
{
	return snprintf(buffer, size, "%d", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, int16_t val)
{
	return snprintf(buffer, size, "%d", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, int32_t val)
{
	return snprintf(buffer, size, "%d", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, uint8_t val)
{
	return snprintf(buffer, size, "%u", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, uint16_t val)
{
	return snprintf(buffer, size, "%u", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, uint32_t val)
{
	return snprintf(buffer, size, "%u", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, float val)
{
	return snprintf(buffer, size, "%.2f", val);
}

inline int to_mqtt_string_helper(char* buffer, const size_t size, char* val)
{
	return snprintf(buffer, size, "%s", val);
}

inline void mqtt_string_helper(const char* buffer, bool& val)
{
	int i =0;
	sscanf( buffer,"%i",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer, int8_t& val)
{
	int i =0;
	sscanf( buffer,"%i",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer, int16_t& val)
{
	int i =0;
	sscanf( buffer,"%i",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer,  int32_t& val)
{
	int i =0;
	sscanf( buffer,"%i",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer, uint8_t& val)
{
	unsigned int i =0;
	sscanf( buffer,"%u",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer,  uint16_t& val)
{
	unsigned int i =0;
	sscanf( buffer,"%u",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer,  uint32_t& val)
{
	unsigned int i =0;
	sscanf( buffer,"%u",&i);
	val = i;
}

inline void mqtt_string_helper(const char* buffer, float& val)
{
	sscanf( buffer,"%f",&val);
}

inline void mqtt_string_helper(const char* buffer,  char* val)
{
	strcpy(val,buffer);
}

#endif //MQTT_HELPER_HPP_
