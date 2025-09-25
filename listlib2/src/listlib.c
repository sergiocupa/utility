#include "../include/listlib.h"
#include <assert.h>
#include <stdlib.h>


#define INITIAL_LIST_LENGTH 100

void list_release(List* _this)
{
	if (_this && _this->IsEnabled)
	{
		if (_this->IsCreated)
		{
			free(_this->Items);
			free(_this);
			_this->IsCreated = false;
		}
		else
		{
			free(_this->Items);
			_this->Items = NULL;
		}
		_this->IsEnabled = false;
	}
}

void list_init(List* _this, int item_size)
{
	_this->IsEnabled = true;
	_this->Count     = 0;
	_this->MaxCount  = INITIAL_LIST_LENGTH;
	_this->ItemSize  = item_size;
	_this->Items     = (void**)malloc(_this->MaxCount * sizeof(void*));
}

List* list_create(int item_size)
{
	List* ar = (List*)malloc(sizeof(List));
	list_init(ar, item_size);
	ar->IsCreated = true;
	return ar;
}

void list_remove(List* _this, void* item)
{
	if (_this && _this->IsEnabled)
	{
		int ix = 0;
		while (ix < _this->Count)
		{
			if (_this->Items[ix] && _this->Items[ix] == item)
			{
				// Repositions items and decrements the counter
				int jx = ix;
				while (jx < _this->Count - 1)
				{
					_this->Items[jx] = _this->Items[jx + 1];
					jx++;
				}
				_this->Count--;
				break;
			}
			ix++;
		}
	}
}

void list_remove_by_index(List* _this, int index)
{
	if (_this && _this->IsEnabled)
	{
		int ix = 0;
		while (ix < _this->Count && index < _this->Count)
		{
			if (ix == index)
			{
				// Repositions items and decrements the counter
				int jx = ix;
				while (jx < _this->Count - 1)
				{
					_this->Items[jx] = _this->Items[jx + 1];
					jx++;
				}
				_this->Count--;
				break;
			}
			ix++;
		}
	}
}

void list_add(List* _this, void* item, int item_size)
{
	if (_this)
	{
		if (item_size != _this->ItemSize)
		{
			assert(0);
		}

		if (_this->Count >= _this->MaxCount)
		{
			_this->MaxCount = ((_this->Count + item_size) + _this->MaxCount) * 2;
			_this->Items    = (void**)realloc((void**)_this->Items, _this->MaxCount * sizeof(void*));
		}

		_this->Items[_this->Count] = item;
		_this->Count++;
	}
}

