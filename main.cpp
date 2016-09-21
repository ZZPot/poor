#include <windows.h>
#include <stdio.h>

#define POORED      0
#define NO_ACCESS   1
#define NOT_RICH    2
#define STUB_SIZE   0x40

DWORD MakePoor(char* file);
BYTE* MapFile(char* file);

int main(int argc, char* argv[])
{
    if(argc < 2)
        return 1;
    DWORD p = 0, r = 0;
    for(int i = 1; i < argc; i++)
        switch(MakePoor(argv[i]))
        {
        case POORED:
            printf("%s: poored\n", argv[i]);
            p++;
            break;
        case NO_ACCESS:
            printf("%s: no access\n", argv[i]);
            r++;
            break;
        case NOT_RICH:
            printf("%s: not rich\n", argv[i]);
            r++;
            break;
        }
    return 0;
}

DWORD MakePoor(char* file)
{
    BYTE* pBase = MapFile(file);
    if(!pBase)
        return NO_ACCESS;
    IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)pBase;
    if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        UnmapViewOfFile(pBase);
        return NOT_RICH;
    }
    if(sizeof(IMAGE_DOS_HEADER) + STUB_SIZE == pDosHeader->e_lfanew)
    {
        UnmapViewOfFile(pBase);
        return NOT_RICH;
    }
    memset(pBase + sizeof(IMAGE_DOS_HEADER) + STUB_SIZE, 0, pDosHeader->e_lfanew - (sizeof(IMAGE_DOS_HEADER) + STUB_SIZE));
    UnmapViewOfFile(pBase);
    return POORED;
}

BYTE* MapFile(char* file)
{
    HANDLE hFile = CreateFile(file, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return NULL;
    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    if(hFile == NULL)
    {
        CloseHandle(hFile);
        return NULL;
    }
    BYTE* pFile = (BYTE*)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
    CloseHandle(hMap);
    CloseHandle(hFile);
    return pFile;
}
