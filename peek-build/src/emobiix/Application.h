#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "DataObject_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Application_t;
typedef struct Application_t Application;

struct Style_t;

Application *application_load(DataObject *dobj);
void application_setActive(Application *app);
DataObject *application_getCurrentScreen(Application *app);
void application_setCurrentScreen(Application *app, DataObject *screen);
void *application_getCurrentStyle(Application *app);
int application_isIconified(Application *app);
DataObject *application_getDataObject(Application *app);

#ifdef __cplusplus
}
#endif

#endif /* _APPLICATION_H_ */
