#include "stm32f4xx.h"
#include "usart_rev.h"

typedef struct {
    UartTransferState_Typedef State;
    uint8_t         Error;
    uint32_t        TimeMs;
    uint32_t        TimeOut;
    uint16_t        MaxSize;
    uint16_t        ByteCnt;
    uint8_t        *pBaseBuff;
    UartFrameDecode     Decode;
    UartFrameRestartRev RestartRev;
}UartTransferFrame_Typedef;

typedef struct {
    UartTransferFrame_Typedef Rev[UART_TRANSFER_ID_MAX];
    //UartTransferTypedef Send;
    volatile uint32_t   *pCurrTimeMs;
}UartTransfer_Typedef;


static UartTransfer_Typedef UartTransfer;

void UartTransfer_Init(uint32_t *p_currtime)
{
    UartTransferFrame_Typedef *pUartRev;

    for(int i = 0; i < UART_TRANSFER_ID_MAX; i++) {
        pUartRev = &UartTransfer.Rev[i];

        pUartRev->State     = UART_TRANSFER_STATUS_IDLE;
        pUartRev->Error     = 0;
        pUartRev->TimeMs    = 0;
        pUartRev->TimeOut   = 0;
        pUartRev->MaxSize   = 0;
        pUartRev->ByteCnt   = 0;
        pUartRev->pBaseBuff = NULL;
        pUartRev->Decode    = NULL;
        pUartRev->RestartRev= NULL;
    }

    UartTransfer.pCurrTimeMs = p_currtime;
}


void UartTransfer_Regeist(UartTransferID_Typedef id, UartTransferInit_Typedef *p_uart_init)
{
    UartTransferFrame_Typedef *pUartRev;

    pUartRev = &UartTransfer.Rev[id];

    pUartRev->TimeOut    = p_uart_init->TimeOut;
    pUartRev->MaxSize    = p_uart_init->MaxSize;
    pUartRev->pBaseBuff  = p_uart_init->pBaseBuff;
    pUartRev->Decode     = p_uart_init->Decode;
    pUartRev->RestartRev = p_uart_init->RestartRev;
}


BOOL UartTransfer_IsRevFrame(UartTransferID_Typedef id)
{
    UartTransferFrame_Typedef *pUartRev;
    UartTransferError_Typedef error = UART_TRANSFER_ERROR_NO;
    uint16_t length;
    uint32_t time;
    BOOL     result = FALSE;

    pUartRev = &UartTransfer.Rev[id];
    time     = *UartTransfer.pCurrTimeMs;
    length   = pUartRev->ByteCnt;

    if((length > 0) && (time - pUartRev->TimeMs > pUartRev->TimeOut)) {
        pUartRev->State = UART_TRANSFER_STATUS_PROCESSING;

        if(pUartRev->Decode != NULL) {
            result = pUartRev->Decode(pUartRev->pBaseBuff,length);

            if(result != TRUE) {
                UartTransfer_AgainRev(id);
            }
        }
        else {
            error = UART_TRANSFER_ERROR_DECODE_NULL;
        }
    }
    else{
        error = UART_TRANSFER_ERROR_REV_NO_FINISHED;
    }

    pUartRev->Error = error;

    return result;
}


void UartTransfer_RevFromRNE(UartTransferID_Typedef id,uint8_t byte)
{
    UartTransferFrame_Typedef *pUartRev;

    pUartRev = &UartTransfer.Rev[id];

    if(pUartRev->State == UART_TRANSFER_STATUS_PROCESSING) {
        pUartRev->Error = UART_TRANSFER_ERROR_PROCESSING;
        return ;
    }

    if(pUartRev->ByteCnt >= pUartRev->MaxSize) {
        pUartRev->Error = UART_TRANSFER_ERROR_REV_OUT_SIZE;
        return;
    }
    if(pUartRev->pBaseBuff != NULL){
         pUartRev->pBaseBuff[pUartRev->ByteCnt++] = byte;
         pUartRev->TimeMs                         = *UartTransfer.pCurrTimeMs;
    }
    else {
        pUartRev->Error = UART_TRANSFER_ERROR_BASEBUFF_NULL;
    }
}


void UartTransfer_RevFromDMA(UartTransferID_Typedef id,uint16_t size)
{
    UartTransferFrame_Typedef *pUartRev;

    pUartRev = &UartTransfer.Rev[id];

    if(pUartRev->State == UART_TRANSFER_STATUS_PROCESSING) {
        pUartRev->Error = UART_TRANSFER_ERROR_PROCESSING;
        return ;
    }

    if(size > pUartRev->MaxSize) { // not   >=
        pUartRev->Error = UART_TRANSFER_ERROR_REV_OUT_SIZE;
        return;
    }

    pUartRev->TimeMs  = *UartTransfer.pCurrTimeMs;

    pUartRev->ByteCnt = size;
}

uint16_t UartTransfer_GetLength(UartTransferID_Typedef id)
{
    return UartTransfer.Rev[id].ByteCnt;
}

uint8_t *UartTransfer_GetBaseBuff(UartTransferID_Typedef id)
{
    return UartTransfer.Rev[id].pBaseBuff;
}

uint8_t UartTransfer_GetError(UartTransferID_Typedef id)
{
    return UartTransfer.Rev[id].Error;
}

UartTransferState_Typedef UartTransfer_GetState(UartTransferID_Typedef id)
{
    return UartTransfer.Rev[id].State;
}

void UartTransfer_AgainRev(UartTransferID_Typedef id)
{
    UartTransferFrame_Typedef *pUartRev;

    pUartRev = &UartTransfer.Rev[id];

    pUartRev->ByteCnt = 0;
    pUartRev->Error   = UART_TRANSFER_ERROR_NO;
    pUartRev->State   = UART_TRANSFER_STATUS_IDLE;

    if(pUartRev->RestartRev != NULL){
        pUartRev->RestartRev();
    }
}
