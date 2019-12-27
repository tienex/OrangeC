/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "ctypes.h"
#include "Utils.h"

static MEMBLK* freestdmem;
static MEMBLK* globals;
static MEMBLK* locals;
/*static*/ MEMBLK* opts;
static MEMBLK* alias;
static MEMBLK* temps;
static MEMBLK* live;
static MEMBLK* templates;
static MEMBLK* conflicts;



static int globalFlag=1;
static int globalPeak, localPeak, optPeak, tempsPeak, aliasPeak, livePeak, templatePeak, conflictPeak;

#define MINALLOC (128 * 1024)
#define MALIGN (4)

#ifdef _WIN32
#    include <windows.h>
static HANDLE globalHeap;
static HANDLE localHeap;
static HANDLE optHeap;
static HANDLE aliasHeap;
static HANDLE tempHeap;
static HANDLE liveHeap;
static HANDLE templateHeap;
static HANDLE conflictHeap;
#endif
//#define DEBUG
void mem_summary(void)
{
    printf("Memory used:\n");
    printf("\tGlobal Peak %dK\n", (globalPeak + 1023) / 1024);
    printf("\tLocal peak %dK\n", (localPeak + 1023) / 1024);
    printf("\tTemplate peak %dK\n", (templatePeak + 1023) / 1024);
    printf("\tOptimizer peak %dK\n", (optPeak + 1023) / 1024);
    printf("\tTemporary peak %dK\n", (tempsPeak + 1023) / 1024);
    printf("\tAlias peak %dK\n", (aliasPeak + 1023) / 1024);
    printf("\tLive peak %dK\n", (livePeak + 1023) / 1024);
    printf("\tConflict peak %dK\n", (conflictPeak + 1023) / 1024);
    globalPeak = localPeak = optPeak = tempsPeak = aliasPeak = livePeak = conflictPeak = 0;
}
static MEMBLK* galloc(MEMBLK** arena, HANDLE* heap, int size)
{
    MEMBLK* selected;
    int allocsize = size <= MINALLOC ? MINALLOC : (size + (MINALLOC - 1)) & -MINALLOC;
    selected = nullptr;
    if (allocsize == MINALLOC)
    {
        if (freestdmem)
        {
            selected = freestdmem;
            freestdmem = freestdmem->next;
        }
    }
    if (!selected)
    {
#ifdef _WIN32
        if (!*heap)
            *heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
        selected = (MEMBLK*)HeapAlloc(*heap, 0, allocsize + sizeof(MEMBLK) - 1);
#else
        selected = (MEMBLK*)malloc(allocsize + sizeof(MEMBLK) - 1);
#endif
        if (!selected)
            Utils::fatal("out of memory");
        selected->size = allocsize;
    }
    selected->left = selected->size;
#ifdef DEBUG
    memset(selected->m, 0xc4, selected->left);
#else
//    memset(selected->m, 0, selected->left);
#endif
    selected->next = *arena;
    *arena = selected;
    //    printf("%d %d\n", count1, count2);
    return selected;
}
void* memAlloc(MEMBLK** arena, HANDLE* heap, int size)
{
    MEMBLK* selected = *arena;
    void* rv;
    if (!selected || selected->left < size)
    {
        selected = galloc(arena, heap, size);
    }
    rv = (void*)(selected->m + selected->size - selected->left);
    //#ifdef DEBUG
    memset(rv, 0, size);
    //#endif
    selected->left = selected->left - ((size + MALIGN - 1) & -MALIGN);
    return rv;
}
void memFree(MEMBLK** arena, HANDLE* heap, int* peak)
{
    MEMBLK* freefind = *arena;
    long size = 0;
    if (!freefind)
        return;
    while (freefind)
    {
        MEMBLK* next = freefind->next;
#ifdef DEBUG
        memset(freefind->m, 0xc4, freefind->size);
#endif
        size += freefind->size;
#ifndef _WIN32
        if (freefind->size == MINALLOC)
        {
            freefind->next = freestdmem;
            freestdmem = freefind;
        }
        else
        {
            free(freefind);
        }
#endif
        freefind = next;
    }
#ifdef _WIN32
    HeapDestroy(*heap);
    *heap = nullptr;
#endif
    *arena = 0;
    if (size > *peak)
        *peak = size;
}
void* globalAlloc(int size) { return memAlloc(&globals, &globalHeap, size); }
void globalFree(void)
{
    memFree(&globals, &globalHeap, &globalPeak);
    globalFlag = 1;
}
void* localAlloc(int size) { return memAlloc(&locals, &localHeap, size); }
void localFree(void) { memFree(&locals, &localHeap, &localPeak); }
void* Alloc(int size)
{
#ifndef PARSER_ONLY
    if (!globalFlag)
        return memAlloc(&locals, &localHeap, size);
#endif
    return memAlloc(&globals, &globalHeap, size);
}
void* oAlloc(int size) { return memAlloc(&opts, &optHeap, size); }
void oFree(void) { memFree(&opts, &optHeap, &optPeak); }
void* aAlloc(int size) { return memAlloc(&alias, &aliasHeap, size); }
void aFree(void) { memFree(&alias, &aliasHeap, &aliasPeak); }
void* tAlloc(int size) { return memAlloc(&temps, &tempHeap, size); }
void tFree(void) { memFree(&temps, &tempHeap, &tempsPeak); }
void* cAlloc(int size) { return memAlloc(&conflicts, &conflictHeap, size); }
void cFree(void) { memFree(&conflicts, &conflictHeap, &conflictPeak); }
void* sAlloc(int size) { return memAlloc(&live, &liveHeap, size); }
void sFree(void) { memFree(&live, &liveHeap, &livePeak); }
void IncGlobalFlag(void) { globalFlag++; }
void DecGlobalFlag(void) { globalFlag--; }
void SetGlobalFlag(int flag) { globalFlag = flag; }
int GetGlobalFlag(void) { return globalFlag; }
char* litlate(const char* name)
{
    int l;
    char* rv = (char*)Alloc((l = strlen(name)) + 1);
    memcpy(rv, name, l);
    return rv;
}
LCHAR* wlitlate(const LCHAR* name)
{
    const LCHAR* p = name;
    int count = 0;
    LCHAR* rv;
    while (*p)
        p++, count++;
    IncGlobalFlag();
    rv = (LCHAR*)Alloc((count + 1) * sizeof(LCHAR));
    DecGlobalFlag();
    p = name;
    count = 0;
    while (*p)
        rv[count++] = *p++;
    rv[count] = 0;
    return rv;
}