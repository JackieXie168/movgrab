#include "includes.h"


ListNode *GetListHead(ListNode *Node)
{
if (! Node) return(NULL);
return(Node->Head);
}



int CountItemsInList(ListNode *List)
{
ListNode *Head;
int *intptr;

if (! List) return(0);
Head=GetListHead(List);
intptr=(int *) Head->Item;

return(*intptr);

}




int SetNoOfItems(ListNode *LastItem, int val)
{
ListNode *Head;
int *intptr;

Head=GetListHead(LastItem);
if (LastItem->Next==NULL) 
   Head->Prev=LastItem; /* The head Item has its Prev as being the last item! */
intptr=(int *) Head->Item;
(*intptr)=val;

return(*intptr);
}



int IncrNoOfItems(ListNode *LastItem)
{
ListNode *Head;
int *intptr;

Head=GetListHead(LastItem);
if (LastItem->Next==NULL) 
   Head->Prev=LastItem; /* The head Item has its Prev as being the last item! */
intptr=(int *) Head->Item;
(*intptr)++;

return(*intptr);
}

int DecrNoOfItems(ListNode *LastItem)
{
ListNode *Head;
int *intptr;

Head=GetListHead(LastItem);
if (LastItem->Next==NULL) Head->Prev=LastItem->Prev; /* The head Item has its Prev as being the last item! */
intptr=(int *) Head->Item;
(*intptr)--;
return(*intptr);
}


ListNode *CreateEmptyList()
{
ListNode *TempPtr;

TempPtr=(ListNode *)calloc(1,sizeof(ListNode));
TempPtr->Head=TempPtr;
TempPtr->Prev=TempPtr;
TempPtr->Next=NULL;
TempPtr->Item=calloc(1,sizeof(int));

(*(int *)TempPtr->Item)=0;
return(TempPtr);
}

void ClearList(ListNode *ListStart, LIST_ITEM_DESTROY_FUNC ItemDestroyer)
{
  ListNode *Curr,*Next;

  if (! ListStart) return; 
  Curr=GetNextListItem(ListStart);
  while (Curr !=NULL) 
  {
      Next=Curr->Next;
      if (ItemDestroyer && Curr->Item) ItemDestroyer(Curr->Item);
      DestroyString(Curr->Tag);
     free(Curr);
     Curr=Next;
  }
ListStart->Next=NULL;
ListStart->Head=ListStart;
ListStart->Prev=ListStart;
SetNoOfItems(ListStart,0);
}

void DestroyList(ListNode *ListStart, LIST_ITEM_DESTROY_FUNC ItemDestroyer)
{
  ListNode *Curr,*Next;

  if (! ListStart) return; 
  ClearList(ListStart, ItemDestroyer);
  free(ListStart->Item);
  free(ListStart);
}


ListNode *CloneList(ListNode *ListStart, LIST_ITEM_CLONE_FUNC ItemCloner)
{
  ListNode *Curr,*NewList;
  void *Item;

  if (! ItemCloner) return(NULL); 
  NewList=CreateEmptyList();

  Curr=GetNextListItem(ListStart);
  while (Curr !=NULL) 
  {
     if (ItemCloner)
     {
	     Item=ItemCloner(Curr->Item);
	     AddNamedItemToList(NewList,Curr->Tag,Item);
     }
     Curr=GetNextListItem(Curr);
  }
  return(NewList);
}

ListNode *ListAddNamedItemAfter(ListNode *ListStart,const char *Name,void *Item)
{
ListNode *Curr;

if (ListStart==NULL) return(NULL);

Curr=ListStart;
Curr->Next=(ListNode *) calloc(1,sizeof(ListNode)); 
Curr->Next->Prev=Curr;
Curr=Curr->Next;
Curr->Item=Item;
Curr->Head=GetListHead(ListStart);
Curr->Next=NULL;
if (Name) Curr->Tag=CopyStr(NULL,Name);

IncrNoOfItems(Curr);
return(Curr);
}



ListNode *AddNamedItemToList(ListNode *ListStart,const char *Name,void *Item)
{
ListNode *Head, *Curr;

Curr=GetLastListItem(ListStart);
if (Curr==NULL) return(Curr);
return(ListAddNamedItemAfter(Curr,Name,Item));
}



ListNode *AddItemToList(ListNode *ListStart,void *Item)
{
return(AddNamedItemToList(ListStart,NULL,Item));
}


ListNode *InsertNamedItemIntoList(ListNode *InsertNode, const char *Name, void *Item)
{
ListNode *NewItem, *Next;

Next=InsertNode->Next;
NewItem=(ListNode *) calloc(1,sizeof(ListNode)); 
NewItem->Item=Item;
NewItem->Prev=InsertNode;
NewItem->Next=Next;
InsertNode->Next=NewItem;
NewItem->Head=InsertNode->Head;
if (Next) Next->Prev=NewItem; /* Next might be NULL! */
IncrNoOfItems(NewItem);
if (StrLen(Name)) NewItem->Tag=CopyStr(NewItem->Tag,Name);
return(NewItem);
}


void OrderedListAddJump(ListNode *From, ListNode *To)
{
int result;

if (! From) return;
if (! To) return;

if (From->Jump)
{
	result=strcmp(From->Jump->Tag, To->Tag);
	if (result > 0)  OrderedListAddJump(From->Next,To);
	else if (result==0)
	{
		 return;
	}
	else
	{
	OrderedListAddJump(From->Next,From->Jump);
	From->Jump=To;
	}
}
else From->Jump=To;
}


ListNode *OrderedListAddNamedItem(ListNode *Head, const char *Name, void *Item)
{
ListNode *NewItem, *Prev, *Curr, *Start;
int count=0, jcount=0, result=-1;


if (! Head) return(NULL);
Prev=Head;
Curr=Head->Next;
Start=Curr;

while (Curr)
{
if (Curr->Jump)
{
	count=0;
	result=strcmp(Curr->Jump->Tag,Name);
	if (result < 0)
	{
		 Curr=Curr->Jump;
		 Prev=Curr->Prev;
		 jcount++;
		if (jcount > 5)
		{
		 OrderedListAddJump(Head->Next, Curr);
		 jcount=0;
		}
	Start=Curr->Next;
	}
}

if (Curr->Tag) result=strcmp(Curr->Tag,Name);
if (result > -1) break;

count++;
if (count > 100)
{
 OrderedListAddJump(Start, Curr);
 count=0;
 Start=Curr->Next;
}

Prev=Curr;
Curr=Curr->Next;
}

NewItem=(ListNode *) calloc(1,sizeof(ListNode)); 
NewItem->Item=Item;
NewItem->Prev=Prev;
NewItem->Next=Prev->Next;
Prev->Next=NewItem;
NewItem->Head=Prev->Head;
if (NewItem->Next) NewItem->Next->Prev=NewItem; /* Next might be NULL! */
IncrNoOfItems(NewItem);
if (StrLen(Name)) NewItem->Tag=CopyStr(NewItem->Tag,Name);


result=CountItemsInList(NewItem);

if ((result % 2000)==0)
{
Curr=GetNthListItem(Head,result / 2);
OrderedListAddJump(Head->Next, Curr);
}


return(NewItem);
}



ListNode *InsertItemIntoList(ListNode *InsertNode, void *Item)
{
return(InsertNamedItemIntoList(InsertNode, NULL, Item));
}


void InsertItemIntoSortedList(ListNode *List, void *Item, int (*LessThanFunc)(void *, void *, void *))
{
ListNode *Curr, *Prev;

Prev=List;
Curr=GetNextListItem(Prev);
while (Curr && (LessThanFunc(NULL, Curr->Item,Item)) )
{
Prev=Curr;
Curr=GetNextListItem(Prev);
}

InsertItemIntoList(Prev,Item);
}


void *DeleteNodeFromList(ListNode *Node)
{
ListNode *Prev, *Next, *Curr;
void *Contents;
int result;

if (Node==NULL)
{
 return(NULL);
}

Curr=Node->Head;
if (Curr) Curr=Curr->Next;
while (Curr)
{
if (Curr->Jump)
{
	if (Curr->Jump==Node) Curr->Jump=NULL;
	/*
	if (strcmp(Curr->Jump->Tag,Node->Tag) > -1)
	{
		Curr=Curr->Jump;
		continue;
	}
	*/
}

Curr=GetNextListItem(Curr);
}

Prev=Node->Prev;
Next=Node->Next;
if (Prev !=NULL) Prev->Next=Next;
if (Next !=NULL) Next->Prev=Prev;


Contents=Node->Item;

DecrNoOfItems(Node);
free(Node);
return(Contents);
}


ListNode *GetNextListItem(ListNode *CurrItem)
{
if (CurrItem !=NULL) return(CurrItem->Next);
else return(NULL);
}

ListNode *GetPrevListItem(ListNode *CurrItem)
{
ListNode *Prev;
if (CurrItem !=NULL)
{
Prev=CurrItem->Prev;
/* Don't return the dummy header! */
if (Prev && (Prev->Prev !=NULL) && (Prev != Prev->Head)) return(Prev);
else return(NULL);
}
else return(NULL);
}


ListNode *GetLastListItem(ListNode *CurrItem)
{
ListNode *Head;


Head=GetListHead(CurrItem);
if (! Head) return(NULL);
/* the dummy header has a 'Prev' entry that points to the last item! */
return(Head->Prev);
}

ListNode *GetNthListItem(ListNode *Head, int n)
{
ListNode *Curr;
int count=0;

if (! Head) return(NULL);

Curr=GetNextListItem(Head);
while (Curr && (count < n))
{
   count++;
   Curr=GetNextListItem(Curr);
}
if (count < n) return(NULL);
return(Curr);
}



void *IndexArrayOnList(ListNode *ListHead)
{
ListNode *Curr;
int count, list_size;
void **PtrArray;

Curr=GetNextListItem(ListHead);  /* Skip past empty list 'header' item */
list_size=0;
while (Curr !=NULL) 
{
Curr=GetNextListItem(Curr);
list_size++;
}
PtrArray=calloc(list_size+1,sizeof(void *));

Curr=GetNextListItem(ListHead);  /* All lists have a dummy header, remember? */
for (count=0;count < list_size; count++)
{
PtrArray[count]=Curr->Item;
Curr=GetNextListItem(Curr);

}
PtrArray[count]=NULL;
return(PtrArray);

}


void *AddItemToArray(void *Array,int size, void *Item)
{
void **PtrArray;


/* two more than number of items in array, one is the new item, and*/
/* one is the terminating null */
PtrArray=Array;
PtrArray=realloc(PtrArray, (size+2) *sizeof(void *));

PtrArray[size]=Item;
PtrArray[size+1]=NULL;
return(PtrArray);
}

void *DeleteItemFromArray(void *Array,int size, int ItemNo)
{
int count;
void **PtrArray;


/* two more than number of items in array, one is the new item, and*/
/* one is the terminating null */
PtrArray=Array;

/* size is actually number of items in array, hence size+1 is the */
/* terminating null. So we include that in the copy */
for (count=ItemNo; count < (size+1); count++)
{
PtrArray[count]=PtrArray[count+1];
}

PtrArray=realloc(PtrArray, (size+1) *sizeof(void *));
return(PtrArray);
}




ListNode *JoinLists(ListNode *List1, ListNode *List2)
{
ListNode *Curr, *StartOfList2;

Curr=List1;
/*Lists all have a dummy header!*/
StartOfList2=List2->Next;

while (Curr->Next !=NULL) Curr=Curr->Next;
Curr->Next=StartOfList2;
StartOfList2->Prev=Curr;

while (Curr->Next !=NULL) Curr=Curr->Next;
return(Curr);
}


void SwapListItems(ListNode *List1, ListNode *List2)
{
void *Item1, *Item2;

Item1=List1->Item;
Item2=List2->Item;
List1->Item=Item2;
List2->Item=Item1;
}


void SortList(ListNode *List, void *Data, int (*LessThanFunc)(void *, void *, void *))
{
ListNode *Curr=NULL, *Prev=NULL;
int sorted=0;

while (! sorted)
{ 
  sorted=1;
  Prev=NULL;
  Curr=GetNextListItem(List);
  while (Curr)
  {
    if (Prev !=NULL)
    {
       if ( (*LessThanFunc)(Data,Prev->Item,Curr->Item) )
       {
         sorted=0;
         SwapListItems(Prev,Curr);
       }
    }

    Prev=Curr;
    Curr=GetNextListItem(Curr);
  }
}

}


ListNode *ListFindNamedItem(ListNode *Head, const char *Name)
{
ListNode *Curr;
int result;

if (! StrLen(Name)) return(NULL);
Curr=GetNextListItem(Head);
while (Curr)
{
   if (Curr->Jump)
   {
		result=strcmp(Curr->Jump->Tag,Name);
		if (result < 0) Curr=Curr->Jump;
   }
   if (Curr->Tag && (strcmp(Curr->Tag,Name)==0)) return(Curr);
   Curr=GetNextListItem(Curr);
}
return(Curr);
}

ListNode *ListFindItem(ListNode *Head, void *Item)
{
ListNode *Curr;

if (! Item) return(NULL);
Curr=GetNextListItem(Head);
while (Curr)
{
   if (Curr->Item==Item) return(Curr);
   Curr=GetNextListItem(Curr);
}
return(Curr);
}

