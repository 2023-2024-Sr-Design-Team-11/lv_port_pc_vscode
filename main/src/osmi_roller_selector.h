#ifndef roller_selector_h
#define roller_selector_h
#include "lvgl/lvgl.h"
#include "lvgl/src/core/lv_obj_class.h"

typedef struct
{
    lv_obj_t *container;

    lv_obj_t* roller_tens;
    lv_obj_t* roller_ones;
    lv_obj_t* roller_tenths;

    lv_obj_t* roller_units;

    float value; // Value in mlPerMinute
} osmi_roller_selector;

void osmi_roller_selector_create(lv_obj_t *parent, osmi_roller_selector *this);
float osmi_roller_get_value(osmi_roller_selector *this);
#endif
