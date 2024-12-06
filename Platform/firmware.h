//--------------------------------------------------
// Все необходимые прошивки для барьера тут
//--------------------------------------------------
#ifndef TESTMODULE_142_FIRMWARE_H
#define TESTMODULE_142_FIRMWARE_H

//--------------------------------------------------
#include <stdint.h>

#include "common.h"

//--------------------------------------------------
#define TM_142_LPA_FW_SIZE 32*1024
#define TM_142_LPA_FW_VERSION_OFFSET TM_142_LPA_FW_SIZE - 4

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void firmware_init(void);

uint8_t firmware_get(data_id_t id, uint8_t *data, uint32_t sz, uint16_t offset);
uint8_t firmware_put(data_id_t id, const uint8_t *data, uint32_t sz, uint16_t offset);

uint8_t firmware_get_major(void);
uint8_t firmware_get_minor(void);
uint8_t firmware_get_patch(void);
uint8_t firmware_get_build(void);

uint8_t firmware_save(void);

/* non-blocking version,
 * just call once and again
 * if it returns 0 - go on
 * if it returns 255 - success
 * otherwise - error
 */
uint8_t firmware_save_nb(void);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
