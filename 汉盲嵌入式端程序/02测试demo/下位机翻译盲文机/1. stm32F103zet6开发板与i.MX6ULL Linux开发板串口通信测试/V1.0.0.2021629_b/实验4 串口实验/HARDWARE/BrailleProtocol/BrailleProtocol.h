#ifndef __BRAILLEPROTOCOL_H
#define __BRAILLEPROTOCOL_H

#include "sys.h"
void SendText(u8* InputText, u8 len);
int32_t UnpackedDot(u8* dotbuf, u8* cnt);
void SendNCK(void);
void SendACK(void);
#endif
