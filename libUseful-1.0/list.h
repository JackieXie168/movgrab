#ifndef LIB_USEFUL_LIST
#define LIB_USEFUL_LIST


#ifdef __cplusplus
extern "C" {
#endif

#define LIST_FLAG_DELETE 1

typedef struct lnode
{
int ItemType;
int Flags;
char *Tag;
void *Item;
struct lnode *Head;
struct lnode *Jump;
struct lnode *Prev;
struct lnode *Next;
} ListNode;

typedef void (*LIST_ITEM_DESTROY_FUNC)(void *);
typedef void *(*LIST_ITEM_CLONE_FUNC)(void *);

ListNode *CreateEmptyList();
void *IndexArrayOnList(ListNode *);
void *AddItemToArray(void *Array,int size, void *Item);
void *DeleteItemFromArray(void *Array,int size, int ItemNo);
void DestroyList(ListNode *, LIST_ITEM_DESTROY_FUNC);
void ClearList(ListNode *, LIST_ITEM_DESTROY_FUNC);
ListNode *AddItemToList(ListNode *,void *);
ListNode *AddNamedItemToList(ListNode *, const char *Name, void *);
ListNode *InsertItemIntoList(ListNode *,void *);
ListNode *InsertNamedItemIntoList(ListNode *,const char *,void *);
ListNode *OrderedListAddNamedItem(ListNode *Head, const char *Name, void *Item);
ListNode *InsertItemIntoSortedList(ListNode *, void *, int (*LessThanFunc)(void *, void *, void *));
void *DeleteNodeFromList(ListNode *);
ListNode *ListAddNamedItemAfter(ListNode *ListStart,const char *Name,void *Item);
ListNode *GetNextListItem(ListNode *);
ListNode *GetPrevListItem(ListNode *);
ListNode *GetListHead(ListNode *);
ListNode *GetLastListItem(ListNode *);
ListNode *GetNthListItem(ListNode *Head, int n);
ListNode *ListFindNamedItem(ListNode *Head, const char *Name);
ListNode *ListFindItem(ListNode *Head, void *Item);
ListNode *JoinLists(ListNode *, ListNode *);
ListNode *CloneList(ListNode *, LIST_ITEM_CLONE_FUNC);
void SortList(ListNode *, void *Data, int (*LessThanFunc)(void *, void *, void *));
void SwapListItems(ListNode *, ListNode *);
int CountItemsInList(ListNode *);

#ifdef __cplusplus
}
#endif


#endif
