/* read tokens from text data stored in ROM */

#include <stdio.h>

char str[1024];
int i;
float f;

void whitespace(char** data)
{
	while(**data && **data==' ') ++*data;
}

void to_whitespace(char** data)
{
	while(**data && **data!=' ') ++*data;
}

char* read_str(char** data)
{
	whitespace(data);
	if(!**data) return 0;

	/* scan string */
	sscanf(*data, "%s", str);
	to_whitespace(data);
	return str;
}

int read_int(char **data)
{
	whitespace(data);
	if(!**data) return 0;

	/* scan int */
	sscanf(*data, "%d", &i);
	to_whitespace(data);
	return i;
}

float read_float(char **data)
{
	whitespace(data);
	if(!**data) return 0;

	/* scan float */
	sscanf(*data, "%f", &f);
	to_whitespace(data);
	return f;
}

