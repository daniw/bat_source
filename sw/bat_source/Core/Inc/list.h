/*
 * list.h
 *
 *  Created on: 09.08.2025
 *      Author: ahorat
 */

#ifndef SRC_LIST_H_
#define SRC_LIST_H_

/**
 * Struct that represents a list. The list implemented so that only pointers to a list entry are moved.
 * This prevents that existing links to the list entries are preserved.
 */
typedef struct
{
	void** list;
	void* memory;
	unsigned int size;
	unsigned int length;
	unsigned int count;
} LIST_STR;

#define list_CheckRemove(data) (data == 0)
#define list_Transfer(liststr, index, back) liststr.list[index] = liststr.list[back]; liststr.list[back] = 0

void list_Init(LIST_STR* liststr, void** list, void* memory, unsigned int size, unsigned int length);
void* list_GetFree(LIST_STR* liststr);
void list_Add(LIST_STR* liststr, void* data);
void list_Remove(LIST_STR* liststr, unsigned int index);
void list_RemoveItem(LIST_STR* liststr, void* data);
void* list_Get(LIST_STR* liststr, unsigned int index);

#endif /* SRC_LIST_H_ */
