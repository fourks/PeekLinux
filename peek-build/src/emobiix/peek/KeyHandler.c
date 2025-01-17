#ifndef SIMULATOR
#include "general.h"
#include "typedefs.h"
#include "KeyMap.h"
#include "Debug.h"
#include "Sounds.h"
#endif

#include "Transport.h"
#include "ConnectionContext.h"
#include "DataObject.h"
#include "Platform.h"
#include "Debug.h"
#include "ApplicationManager.h"
#include "lgui.h"
#include "KeyMappings.h"
#include "RenderManager.h"

#include "List.h"
#include "p_malloc.h"

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif

extern unsigned char pwr_PowerOffMobile   (void);

#include "kpd_cfg.h"
#include "hwtask.h"
#include "emo_kbd.h"
#include "ui_pei.h"
#include "p_sim.h"

#define hCommACI aci_hCommACI
EXTERN T_HANDLE         hCommACI;

#ifdef TI_PS_HCOMM_CHANGE
#define PSENDX(A,B) PSEND(_hComm##A,B)
#else
#define PSENDX(A,B) PSEND(hComm##A,B)
#endif /* TI_PS_HCOMM_CHANGE */

List *netsurf_get_queue()
{
	static List *netsurf_key_queue = NULL;
	if (!netsurf_key_queue)
		netsurf_key_queue = list_new();

	return netsurf_key_queue;
}

static void netsurf_key_enqueue(unsigned short key, unsigned short state)
{
	List *keyqueue = netsurf_get_queue();
	unsigned int keystate = state << 16 | key;

	list_append(keyqueue, (void *)keystate);
}

typedef UINT16 RegIdT;
void KeyPad_Init(void);

#ifdef TI_PS_HCOMM_CHANGE
#define PSENDX(A,B) PSEND(_hComm##A,B)
#else
#define PSENDX(A,B) PSEND(hComm##A,B)
#endif /* TI_PS_HCOMM_CHANGE */

typedef enum
{
    BAL_KEY_PRESS,
    BAL_KEY_RELEASE,
    BAL_KEY_HOLD,
    BAL_KEY_STATUS_NUM
}BalKeyStatusT;

int netsurf_start_flag = 0;
extern ConnectionContext *connectionContext;

void UiHandleKeyEvents(UINT32 MsgId, int UiKeyId)
{
		T_EMOBIIX_NETSURF_START *netStart;

    //emo_printf("UiHandleKeyEvents MsgId=%d KeyId=%d" NL, MsgId, UiKeyId);
	
		/* Snap key press time for idle timeout */
		blightSnaptime();
		//memmap();
		
		if(UiKeyId == SYS_PWR_KEY) 
		{
			emo_printf("Starting power off\n");
			pwr_PowerOffMobile();
			while(1); // Shouldn't get here
		}

		if(netsurf_start_flag) {
			netsurf_key_enqueue(UiKeyId, MsgId);
			return;
		}
		switch(UiKeyId) {
			case SYS_SHIFT:
				/* Dont pass through shift.. wait for combo */
				break;
			case SYS_WHEEL_BACK:
			case SYS_WHEEL_FORWARD:
				//emo_printf("UiHandleKeyEvents() Passing to manager key [%c]", MapKeyToInternal(UiKeyId));
				manager_handleKey(MapKeyToInternal(UiKeyId));
				break;
			default:
				if (MsgId == BAL_KEY_PRESS)
				{
					//emo_printf("UiHandleKeyEvents() Passing to manager key [%c]", MapKeyToInternal(UiKeyId));
					manager_handleKey(MapKeyToInternal(UiKeyId));
				}
		}
	//			connectionContext_loopIteration(connectionContext);
        updateScreen();
}

void hwPassKeyEvent(RegIdT RegId, UINT32 MsgId,void *MsgBufferP)
{
  T_EMOBIIX_KEY_EVENT *msg;
  msg = P_ALLOC(EMOBIIX_KEY_EVENT);
	msg->key = *((int *)MsgBufferP);
	msg->state = MsgId;
  PSENDX(ACI, msg);
}

void netsurfStart(void) 
{
	netsurf_start_flag = 1;
	select_peek_plotters();
	netsurf_redraw();
	netsurf_resume();
}


void KeyPad_Init(void) {

     if(BalKeypadRegister(hwPassKeyEvent) == -1) {
        emo_printf("KeypadRegister() Failed to register handler");
        return;
     }
}

