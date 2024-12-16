//--------------------------------------------------
// Режимы работы барьера для внешних программ
//--------------------------------------------------
#ifndef LPA_142_MODES_H
#define LPA_142_MODES_H

//--------------------------------------------------
#define SENSOR_TYPE_MASK  (1 << 0)
#define OUTPUT_TYPE_MASK  (1 << 1)
#define INV_OUT_MASK      (1 << 2)
#define INV_ERR_MASK      (1 << 3)

#define MODE_MASK    0x0F

#define SENSOR_TYPE_NAMUR (MODE_MASK & SENSOR_TYPE_MASK)
#define SENSOR_TYPE_SWITCH 0
#define OUTPUT_TYPE_BOT   (MODE_MASK & OUTPUT_TYPE_MASK)
#define OUTPUT_TYPE_TOP    0
#define DIRECT_OUT        (MODE_MASK & INV_OUT_MASK)
#define INVERTED_OUT       0
#define DIRECT_ERR        (MODE_MASK & INV_ERR_MASK)
#define INVERTED_ERR       0

#define DEFAULT_MODE 'F'

//--------------------------------------------------
#endif
