#include "general.h"
#include "typedefs.h"
#include "nucleus.h"
#include "Task.h"
#include "UITask.h"
#include "mfw_mfw.h"
#include "mfw_mme.h"
#include "Debug.h"
#include "lgui.h"

#include "Transport.h"
#include "ConnectionContext.h"
#include "Platform.h"

#include "DataObject.h"
#include "Application.h"
#include "ApplicationManager.h"
#include "RenderManager.h"

#include "gdi.h"
#include "dspl.h"
#include "p_sim.h"

#include "vsi.h"
#include "system_battery.h"
#include "exeapi.h"
#include "hwtimer.h"

#ifndef SIMULATOR
#include "dspl.h"
#endif

#define BWIDTH 320
#define BHEIGHT 240

#pragma DATA_SECTION(lcd_frame_buffer, ".r2dbuffer")
char lcd_frame_buffer[BWIDTH * BHEIGHT * 2];


#define hCommACI aci_hCommACI
EXTERN T_HANDLE         hCommACI; 

#ifdef TI_PS_HCOMM_CHANGE
#define PSENDX(A,B) PSEND(_hComm##A,B)
#else
#define PSENDX(A,B) PSEND(hComm##A,B)
#endif /* TI_PS_HCOMM_CHANGE */




typedef struct 
{
	UINT16 start_x;
	UINT16 start_y;
	UINT16 end_x;
	UINT16 end_y;
} lcd_fb_coordinates;


void setRecvProcess(int recv);
extern void emo_BitBlt(int x1, int y1, int x2, int y2);
void updateScreen(void);
void UIInit(void);
ConnectionContext *connectionContext;

DataObject *systemAppObject;
Application *systemApplication;

#define UI_SYNC_LOOP 3 * 1000

void ui_timer_cb(tDS *tm, void *data)
{
	connectionContext_loopIteration(connectionContext);
	timerStart(tm, UI_SYNC_LOOP);
}


void uiAppConn(void *connData)
{
	Endpoint *ep;
	URL *url;
	Transport *transport;
	tDS *tm = timerCreate(ui_timer_cb, NULL);

	emo_printf("uiAppConn()");

	url = url_parse("tcp://10.150.9.6:12345/dataobject", URL_ALL);

	transport = transport_get(url->scheme);
	if (transport == NULL) {
		emo_printf("No 'tcp' transport available");
		return;
	}

	ep = transport->socket();
	transport->connect(ep, url);

	connectionContext = connectionContext_new(ep);
	if(!connectionContext) {
		emo_printf("uiAppConn() Failed to allocate connectionContext");
		return;
	}
	if(!simAutoDetect())
		setRecvProcess(0);
	//connectionContext_syncRequest(connectionContext, url);
	//connectionContext_loopIteration(connectionContext);

	timerStart(tm, UI_SYNC_LOOP);
}


void uiAppSent()
{
	emo_printf("uiAppSent() Looping...");
	connectionContext_loopIteration(connectionContext);
}

static int recvProcess = 0;

void setRecvProcess(int recv)
{
	recvProcess = recv;
}

int getRecvProcess(void)
{
	return recvProcess;
}

void uiAppRecv(void *recvData)
{
	T_EMOBIIX_WRITEMSG *writeMsg = (T_EMOBIIX_WRITEMSG *) recvData;
	int status = 1;

	emo_printf("uiAppRecv()");

	connectionContext_setBuffer(connectionContext, (char *)writeMsg->data, writeMsg->size);

	while (status > 0)
	{
		status = connectionContext_consumePacket(connectionContext);
		emo_printf("uiAppRecv() Consumed %d bytes" NL, status);
	}

	connectionContext_loopIteration(connectionContext);
	updateScreen();
	/* mfw -> custom menu
	if(emoMenu_get_window())
		SEND_EVENT(emoMenu_get_window(), SCREEN_UPDATE, 0, 0);
	*/
	if(!simAutoDetect())
		setRecvProcess(0);
}

extern void emo_FullDraw(void);

GLOBAL BOOL appdata_response_cb (ULONG opc, void * data)
{
	//emo_printf("appdata_response_cb(): %d (%08X)", opc, opc);
	char *argv = "emobiix";

	switch(opc) 
	{
		case EMOBIIX_SOCK_RECV: // Data received 
			emo_printf("appdata_response_cb(): APP_DATA_RECV");
			uiAppRecv(data);
			/* Free the p_malloc'd buffer that we created in app_core */
			p_free(((T_EMOBIIX_SOCK_RECV *)data)->data);
			return TRUE;

		case EMOBIIX_SOCK_CREA: // Sock created
			emo_printf("appdata_response_cb(): APP_DATA_CREA");	
			return TRUE;

		case EMOBIIX_SOCK_SENT: // Sock data sent
			emo_printf("appdata_response_cb(): APP_DATA_SENT");
			uiAppSent();
			return TRUE;

		case EMOBIIX_SOCK_CONN: // Sock connected
			emo_printf("appdata_response_cb(): APP_DATA_CONN");
			uiAppConn(data);
			return TRUE;

		case EMOBIIX_SOCK_DCON: // Sock disconnected
			emo_printf("appdata_response_cb(): APP_DATA_DCON");
			return TRUE;

    case EMOBIIX_NETSURF_START:
		{
			emo_printf("appdata_response_cb(): start_netsurf");
			emo_FullDraw();
			return TRUE;
		}
		
		case EMOBIIX_KEY_EVENT:
		{
			//emo_printf("appdata_response_cb(): key_event");
			UiHandleKeyEvents(((T_EMOBIIX_KEY_EVENT *)data)->state, ((T_EMOBIIX_KEY_EVENT *)data)->key);
			return TRUE;
		}
	 default:
			break;
	}

	/* Not handled by this extention */
	return FALSE;
}

U8* emo_LCD_bitmap(void)
{
	return ((U8*)lcd_frame_buffer);
}

void emo_BitBlt(int x1, int y1, int x2, int y2)
{
	static lcd_fb_coordinates coord;

	//emo_printf("Flipping partial screen: %d %d %d %d", x1, y1, x2, y2);

	coord.start_x = x1;
	coord.start_y = y1;
	coord.end_x =  x2;
	coord.end_y = y2;

	if(simAutoDetect())
		lcd_display(0, emo_LCD_bitmap(), &coord);
}

void emo_SendFullDraw(void)
{
	void *msg;
	msg = P_ALLOC(EMOBIIX_NETSURF_START);
	PSENDX(ACI, msg);
}

void emo_FullDraw(void)
{
	/*
	manager_resolveLayout();
	lgui_set_dirty();
	updateScreen();
	*/

	manager_focusBootApp();
}

void updateScreen(void) 
{
#ifndef SIMULATOR
	int index, upper;
	extern int netsurf_start_flag;

	if(netsurf_start_flag)
		return;

	renderman_flush();
	manager_drawScreen();

	if (!lgui_is_dirty())
		return;

	upper = lgui_index_count();
	if (upper == 0) 
	{
		emo_BitBlt(0, 0, BWIDTH, BHEIGHT);
	} 
	else 
	{
		int i;
		Rectangle *rect;
		for (index = 0; index < upper; ++index) 
		{
			rect = lgui_get_region(index);
			emo_BitBlt(rect->x, rect->y, rect->x + rect->width, rect->y + rect->height);
		}
	}

	lgui_blit_done();
#endif
}

void display_init()
{
	int ret;
	emo_printf("Initializing display driver");

	if(simAutoDetect())
		ret = lcd_initialization(0);
	else 
		ret = 0;

	if (ret != 0)
		emo_printf("Display driver initialization failed: %d", ret);

	memset(&lcd_frame_buffer, 0xff, 320*240*2);
	emo_BitBlt(0,0,320,240);
}

static int uiStatus = 0;
int uiGetStatus(void) 
{
	return uiStatus;
}

void UIInit(void)
{	
	if(simAutoDetect())
		backlightInit();

	display_init();

	TCCE_Task_Sleep(500); 

	dataobject_platformInit();
	renderman_init();

	lgui_attach(&lcd_frame_buffer);
	manager_init();

	lgui_set_dirty();
	updateScreen();

	BalKeypadInit(0,0,4);
	KeyPad_Init();

	uiStatus = 1;
	return;
}

