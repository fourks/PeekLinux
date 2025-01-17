#include "stdarg.h"
#include "stdio.h"
#include "rvf_api.h"
#include "rv_trace.h"
#include "rvm_use_id_list.h"
#include "nucleus.h"

typedef long off_t;

unsigned int mfwMmeDrvFlag;
static char buf[76] = {0};

#pragma NO_HOOKS (emo_printf)

void emo_printf( const char* fmt, ...) {
#ifndef EMO_PROD
	va_list ap;
	NU_TASK *tp;

	va_start(ap, fmt);
	tp =  TCC_Current_Task_Pointer();

	if(tp) 
	{
		int len;
		sprintf(buf, "[%s]", tp->tc_name);
		len = strlen(buf);
		vsnprintf(buf + len, sizeof(buf) - 1 - len, fmt, ap);
	} 
	else 
		vsnprintf(buf, sizeof(buf) - 1, fmt, ap);

	buf[60] = '\0';
	rvf_send_trace(buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID);

	va_end(ap);
	TCCE_Task_Sleep(2);
#endif
}

void emo_printlock(char *msg) {
	NU_TASK *tp;

	tp =  TCC_Current_Task_Pointer();

	buf[0] = 0;
	if(tp) {
		strcpy(buf, "[");
		strcat(buf, tp->tc_name);
		strcat(buf, "] ");
	} 
	strncat(buf, msg, 75 - strlen(buf));
	buf[75] = '\0';
	rvf_send_trace (buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID );
}

void emo_fprintf(FILE *file, const char* fmt, ...) {
#ifndef EMO_PROD
	va_list ap;
	NU_TASK *tp;

	if(file->fd > 2) {
		emo_printf("______EMO_FPRINTF USED FOR FILE______");
		while(1) {
			TCCE_Task_Sleep(200);
		}
	}
	va_start(ap, fmt);
	tp =  TCC_Current_Task_Pointer();

	if(!tp) {
		vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
	} else {
		int len;
		sprintf(buf, "[%s]", tp->tc_name);
		len = strlen(buf);
		vsnprintf(buf + len, sizeof(buf) - 1 - len, fmt, ap);
	}
	buf[75] = '\0';
	rvf_send_trace (buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
		va_end(ap);
	TCCE_Task_Sleep(2);
#endif
}

void bal_trace(char *msg) {
	//emo_printf("%s", msg);
}

void bal_printf(const char* fmt, ...) {
#ifndef EMO_PROD
	/* only used for kpd stuff and we know its working
		 va_list ap;
		 NU_TASK *tp;

		 va_start(ap, fmt);
		 tp =  TCC_Current_Task_Pointer();

		 if(!tp) {
		 vsprintf(buf, fmt, ap);
		 } else {
		 sprintf(buf, "Task [%s] ", tp->tc_name);
		 vsprintf(buf+strlen(buf), fmt, ap);
        }
        rvf_send_trace (buf,strlen(buf)+1,NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
        va_end(ap);
        TCCE_Task_Sleep(2);
*/
#endif
}

/* Default streams for stdlib */
int DStreamopen(const char *path, unsigned flags, int llv_fd) {
	emo_printlock("______________DStreamopen_______________");
	return -1;
}

int DStreamclose(int dev_fd) {
	return 0;
}

int DStreamread(int dev_fd, char *buf, unsigned count) {
	return -1;
}

int DStreamwrite(int dev_fd, const char *buf, unsigned count) {
	return -1;
}

int DStreamunlink(const char *path) {
	return -1;
}

int DStreamrename(const char *old_name, const char *new_name) {
	return -1;
}

off_t DStreamlseek(int dev_fd, off_t offset, int origin) {
	return -1;
}

