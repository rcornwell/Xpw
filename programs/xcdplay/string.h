/*
 * XCdplay: Plays a cd.
 *
 * A simple strings class.
 *
 */

/*
 * $Id: $
 *
 * $Log: $
 *
 */

#include <string.h>

class Str {
	char		*value;

public:
	Str()	{value = new char [1]; *value = '\0'; }
	~Str()	{delete [] value; }
	Str(const int len) { value = new char [len+1]; *value = '\0'; }
	Str(const Str &str) { 
		strcpy(value = new char [strlen(str.value)+1], str.value); }
	Str(const char *cptr) { 
		strcpy(value = new char [strlen(cptr)+1], cptr); }
	Str operator+( const Str &str) {
		char *temp = new char [strlen(value) + strlen(str.value) + 1];
		strcpy(temp, value);	
		strcat(temp, str.value);
		Str retval (temp);
		delete [] temp;
		return retval;
	}
	Str& operator=( const Str &str) {
		if (this != &str) {
		    delete [] value;
		    strcpy(value = new char [strlen(str.value) + 1], str.value);
		}
		return *this;
	}

	int operator!=(const Str &str) {
		return strcmp(value, str.value) != 0;
	}
	int operator==(const Str &str) {
		return strcmp(value, str.value) == 0;
	}
	char *cstring() { return value; }	
	void  addline(char *p) {
		int i;
		i = strlen(p) + 1 + strlen(value) + ((*value != '\0')?1:0);
		char *temp = new char [i];
		strcpy(temp, value);
		if (*value != '\0')
		   strcat(temp, "\n");
		strcat(temp, p);
		delete [] value;
		value = temp;
	}
};

