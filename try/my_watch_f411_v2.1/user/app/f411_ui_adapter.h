#ifndef F411_UI_ADAPTER_H
#define F411_UI_ADAPTER_H

#include "watch_core/watch_core.h"

void f411_ui_adapter_init(void);
void f411_ui_adapter_task(void);
void f411_ui_adapter_activate_health_card(WatchCoreHealthFeature feature);
void f411_ui_adapter_back(void);

#endif /* F411_UI_ADAPTER_H */
