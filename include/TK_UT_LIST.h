#ifndef __NMT_UTLIST_H__
#define __NMT_UTLIST_H__

#ifndef int8_t
typedef signed char int8_t ;
#endif

#ifndef uint8_t
typedef unsigned char uint8_t ;
#endif

#ifndef int16_t
typedef signed int int16_t ;
#endif

#ifndef uint16_t
typedef unsigned int uint16_t ;
#endif

#ifndef int32_t
typedef signed long int int32_t ;
#endif

#ifndef uint32_t
typedef unsigned long int uint32_t ;
#endif

#ifndef int64_t
typedef signed long long int int64_t ;
#endif

#ifndef uint64_t
typedef unsigned long long int	uint64_t; 
#endif


#ifndef u_char
typedef unsigned char u_char;
#endif

typedef struct tListHead {
	struct tListHead *Next, *Prev;
} tListHead;

#define INIT_LIST_HEAD(pHead) do { (pHead)->Next = (pHead); (pHead)->Prev = (pHead); } while (0)

/* MACROS */

/* 
* Extracts the object that holds the list head
* pHead : The list head that resides in Obj
* tPbj  : The type which list head resides in Obj
* Obj   : The name of the list head in Obj
*/

#define UT_ListGetObj(pHead, tObj, Obj) ((tObj *)((uint8_t *)(pHead)-(uint64_t)(&((tObj *)0)->Obj)))

/*
* Browse through a list
* Pos : A pointer to a ListHead used as temp object
* ListHead : The actual list to browse
*/

#define UT_ListForEach(Pos, ListHead) for (Pos = (ListHead)->Next; Pos != (ListHead); Pos = Pos->Next)
#define UT_ListForEachSafe(Pos, N, ListHead) for (Pos = (ListHead)->Next, N = Pos->Next; Pos != (ListHead); Pos = N, N = Pos->Next)


/* INLINE FUNCTIONS */
static __inline void __ListAdd(tListHead * New, tListHead * Prev, 
							   tListHead * Next)
{
	Next->Prev = New;
	New->Next = Next;
	New->Prev = Prev;
	Prev->Next = New;
}

static __inline void UT_ListAddTail(tListHead *New, tListHead *List) 
{
	__ListAdd(New, List->Prev, List);
}

static __inline void UT_ListDel(tListHead *ListHead)
{
	ListHead->Next->Prev = ListHead->Prev;
	ListHead->Prev->Next = ListHead->Next;
}

static __inline int UT_ListIsEmpty(tListHead *Head)
{
	return Head->Next == Head;
}

#endif