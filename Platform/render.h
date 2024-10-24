//--------------------------------------------------
// Рисовалка для дисплея. Умеет отображать 1-битовые
// упакованные картинки на буфер кадра и кое-что ещё
//--------------------------------------------------
#ifndef RENDER_H
#define RENDER_H

//--------------------------------------------------
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

//--------------------------------------------------
typedef struct _image_t {
    uint16_t w, h; // width and height in pixels
    bool vertical; // true - 8 bits vertical, false - 8 bits horizontal
    const uint8_t *data;  // image bits
} image_t;
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void render_cls(void);

extern void render_dot(uint8_t x, uint8_t y, bool inverted);
extern void render_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool inverted);
extern void render_image(uint8_t x, uint8_t y, bool inverted, const image_t *img);
extern void render_text(uint8_t x, uint8_t y, bool inverted, const char *fmt, ...);

#ifdef __cplusplus
};
#endif
//--------------------------------------------------
#endif
