/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
					string.h
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

PUBLIC 	void* 		memcpy(void* p_dst, void* p_src, int size);
PUBLIC 	void 		memset(void* p_dest, char ch, int size);
PUBLIC  char* 		strcpy(void* p_dest, void* p_src);
PUBLIC 	int 		strlen(const void* str);
PUBLIC 	void 		debug(); 

#define phys_copy 	memcpy
#define phys_set 	memset
