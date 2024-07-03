#ifndef MEM_H
#define MEM_H

#define MEM_POLICY_FIRSTFIT 0
#define MEM_POLICY_BESTFIT  1
#define MEM_POLICY_WORSTFIT 2

int Mem_Init(int size, int policy);

void *Mem_Alloc(int size);

int Mem_Free(void *ptr);

int Mem_IsValid(void *ptr);

int Mem_GetSize(void *ptr);

float Mem_GetFragmentation();

#endif /*MEM_H*/
