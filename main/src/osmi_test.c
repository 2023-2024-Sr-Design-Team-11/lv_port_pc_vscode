#include "osmi_test.h"
#include "lvgl/src/widgets/lv_label.h"

void osmi_test_create(lv_obj_t *parent, osmi_test_t *this)
{
    this->container = lv_obj_create(parent);
    lv_obj_set_flex_flow(this->container, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(this->container, 100,25);

    this->group_label = lv_label_create(this->container); // create label.
    if(this->group_label == NULL) return;

    lv_label_set_text_fmt(this->group_label, "Freedom");
}
