#ifndef _included_endianio_h_
#define _included_endianio_h_

#include <stdio.h>

#include "datatype.h"

BYTE GetByte(FILE *fp);
WORD GetLittleWord(FILE *fp);
DWORD GetLittleDword(FILE *fp);

VOID PutByte(BYTE v, FILE *fp);
VOID PutLittleWord(WORD v, FILE *fp);
VOID PutLittleDword(DWORD v, FILE *fp);

#endif /* _included_endianio_h_ */
