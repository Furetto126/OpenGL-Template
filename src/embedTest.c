#include <stdio.h>

void sus()
{
        volatile const char a[] = {
#embed "main.cpp"
        , '\0'};
}