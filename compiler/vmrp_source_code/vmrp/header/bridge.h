#ifndef __VMRP_BRIDGE_H__
#define __VMRP_BRIDGE_H__

#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"

typedef struct BridgeMap BridgeMap;

typedef void (*BridgeCB)(struct BridgeMap *o, uc_engine *uc);
typedef void (*BridgeInit)(struct BridgeMap *o, uc_engine *uc, uint32_t addr);

typedef enum BridgeMapType {
    MAP_DATA,
    MAP_FUNC,
} BridgeMapType;

typedef struct BridgeMap {
    uint32_t pos;
    BridgeMapType type;
    char *name;
    BridgeInit initFn;
    BridgeCB fn;
    uint32_t extraData;
} BridgeMap;

#define BRIDGE_FUNC_MAP(offset, mapType, field, init, func, extraData) \
    { offset, mapType, #field, init, func, extraData }

/* 需要外部实现的接口 */
// 向屏幕输出图像
extern void guiDrawBitmap(uint16_t *bmp, int32_t x, int32_t y, int32_t w, int32_t h);
extern int32_t timerStart(uint16_t t);
extern int32_t timerStop();
extern int32_t editCreate(const char *title, const char *text, int32_t type, int32_t max_size);
extern int32 editRelease(int32 edit);
extern char *editGetText(int32 edit);

uc_err bridge_init(uc_engine *uc);
uc_err bridge_ext_init(uc_engine *uc);

int32_t bridge_dsm_init(uc_engine *uc);
int32_t bridge_dsm_mr_start_dsm(uc_engine *uc, char *filename, char *ext, char *entry);
int32_t bridge_dsm_mr_pauseApp(uc_engine *uc);
int32_t bridge_dsm_mr_resumeApp(uc_engine *uc);
int32_t bridge_dsm_mr_timer(uc_engine *uc);
int32_t bridge_dsm_mr_event(uc_engine *uc, int32_t code, int32_t p0, int32_t p1);
int32_t bridge_dsm_network_cb(uc_engine *uc, uint32_t addr, int32_t p0, uint32_t p1);

#endif
