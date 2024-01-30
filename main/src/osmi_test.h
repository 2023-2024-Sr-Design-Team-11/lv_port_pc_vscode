#include "lvgl/lvgl.h"
#include "lvgl/src/core/lv_obj_class.h"

typedef struct
{
    lv_obj_t *container;

    lv_obj_t *group_label;

} osmi_test_t;

void osmi_test_create(lv_obj_t *parent, osmi_test_t *this);
