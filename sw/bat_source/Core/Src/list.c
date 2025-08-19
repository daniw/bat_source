/*
 * list.c
 *
 *  Created on: 09.08.2025
 *      Author: ahorat
 */

#include "list.h"

/**
 * @Brief Initializes a list.
 * @param liststr pointer to the list struct.
 * @param list pointer to the list of pointers.
 * @param memory pointer to the array that reserves the memory for the list.
 * @param size memory size of a list entry.
 * @param length maximum entries in the list.
 */
void list_Init(LIST_STR* liststr, void** list, void* memory, unsigned int size, unsigned int length)
{
	unsigned int i;
	liststr->list = list;
	liststr->memory = memory;
	liststr->size = size;
	liststr->length = length;
	liststr->count = 0;
	for (i = 0; i < length; i++)
	{
		liststr->list[i] = 0;
	}
}

/**
 * @brief Returns the next free spot in the list.
 * @param liststr pointer to the list struct.
 * @return the pointer to the free spot.
 */
void* list_GetFree(LIST_STR* liststr)
{
	char found;
	unsigned int i, t;
	void* ret = liststr->memory;
	for (i = 0; i < liststr->length; i++)
	{
		found = 0;
		for (t = 0; t < liststr->count; t++)
		{
			if (ret == liststr->list[t])
			{
				found = 1;
				break;
			}
		}
		if (found == 0)
		{
			return ret;
		}
		ret += liststr->size;
	}
	return 0;
}

/**
 * @brief Adds a list entry to the list.
 * @param liststr pointer to the list struct.
 * @param data pointer to the data to be added.
 */
void list_Add(LIST_STR* liststr, void* data)
{
	unsigned int back = liststr->count;
	liststr->list[back] = data;
	liststr->count = back + 1;
}

/**
 * @brief Removes a list entry.
 * @param liststr pointer to the list struct.
 * @param index of the entry to remove.
 */
void list_Remove(LIST_STR* liststr, unsigned int index)
{
	liststr->list[index] = 0;
	if (index == (liststr->count - 1))
	{
		liststr->count--;
	}
}

/**
 * @brief Removes a list entry.
 * @param liststr pointer to the list struct.
 * @param data pointer to the entry to be removed.
 */
void list_RemoveItem(LIST_STR* liststr, void* data)
{
	unsigned int i;
	for (i = 0; i < liststr->count; i++)
	{
		if (liststr->list[i] == data)
		{
			list_Remove(liststr, i);
			break;
		}
	}
}

/**
 * @brief Returns a list entry and cleans up gaps in the list.
 * @param liststr pointer to the list struct.
 * @param index of the list entry to return.
 * @return pointer to the list entry
 */
void* list_Get(LIST_STR* liststr, unsigned int index)
{
	unsigned int i = index;
	while ((i < liststr->count) && (list_CheckRemove(liststr->list[i])))
	{
		i++;
	}
	if (i >= liststr->count)
	{
		liststr->count = index;
		return 0;
	}
	else
	{
		if (i != index)
		{
			liststr->list[index] = liststr->list[i];
			liststr->list[i] = 0;
			if (i == (liststr->count - 1))
			{
				liststr->count = index + 1;
			}
		}
	}
	return liststr->list[index];
}
