#ifndef LIST_LIB_H
#define LIST_LIB_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include "../../platformlib/include/platform.h"


typedef struct _List
{
	bool   IsCreated;
	bool   IsEnabled;
	int    MaxCount;
	int    ItemSize;
	int    Count;
	void** Items;
}
List;


PLATFORM_API void  list_release(List* _this);
PLATFORM_API void  list_init(List* _this, int item_size);
PLATFORM_API List* list_create(int item_size);
PLATFORM_API void  list_remove(List* _this, void* item);
PLATFORM_API void  list_remove_by_index(List* _this, int index);
PLATFORM_API void  list_add(List* _this, void* item, int item_size);


#ifdef __cplusplus
}
#endif

#endif /* LIST_LIB */