
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "lv_drv_conf.h"
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#if USE_SDL
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lv_drivers/sdl/sdl.h"
#elif USE_X11
#include "lv_drivers/x11/x11.h"
#endif

// #include "lv_drivers/display/monitor.h"
// #include "lv_drivers/indev/mouse.h"
// #include "lv_drivers/indev/keyboard.h"
// #include "lv_drivers/indev/mousewheel.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static void hal_deinit(void);
static void *tick_thread(void *data);

/**********************
 *  STATIC VARIABLES
 **********************/
static pthread_t thr_tick;    /* thread */
static bool end_tick = false; /* flag to terminate thread */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/
int flowrate;
int unit;

/**********************
 *  STATIC PROTOTYPES
 **********************/
// Label for current rate.
static lv_obj_t *rateLabel;

// Label for delivering state.
static lv_obj_t *statusLabel;
// Label for current rate.
static lv_obj_t *rateLabel;

// Label for delivering state.
static lv_obj_t *statusLabel;

static lv_obj_t* test_roller;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static void btn_event_Pause(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED)
  {
    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text(statusLabel, "Paused");
  }
}

static void btn_event_Start(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED)
  {
    /*Get the first child of the button which is the label and change its text*/
    lv_label_set_text(statusLabel, "Delivering");
  }
}

static void UpdateFlowText()
{
  char flowtext[32];
  char units[16];
  switch (unit)
  {
  case 0:
    strcpy(units, "ml/sec");
    break;
  case 1:
    strcpy(units, "ml/min");
    break;
  case 2:
    strcpy(units, "ml/h");
    break;
  default:
    strcpy(units, "ml/sec");
    break;
  }
  sprintf(flowtext, "Rate: %d %s", flowrate, units);
  lv_label_set_text(rateLabel, flowtext);
}

static void UpdateRate()
{
  float rate = flowrate;
  switch (unit)
  {
  case 0:
    rate = rate * 60;
    break;
  case 1:
    rate = rate;
    break;
  case 2:
    rate = rate / 60;
    break;
  default:
    break;
  }
}

static void roller_event_UpdateRateHun(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED)
  {
    int hundreds = lv_roller_get_selected(obj);
    flowrate = (flowrate % 100) + (hundreds * 100);
    UpdateFlowText();
  }
}

static void roller_event_UpdateRateTen(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED)
  {
    int tens = lv_roller_get_selected(obj);
    flowrate = ((flowrate / 100) * 100) + tens * 10 + (flowrate % 10);
    UpdateFlowText();
  }
}

static void roller_event_UpdateRateOne(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED)
  {
    int ones = lv_roller_get_selected(obj);
    flowrate = ((flowrate / 10) * 10) + ones;
    UpdateFlowText();
  }
}

static void roller_event_UpdateRateUnit(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED)
  {
    unit = lv_roller_get_selected(obj);
    UpdateFlowText();
  }
}

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init();

  /**OSMI BASICS*/

  lv_obj_t *startbtn = lv_btn_create(lv_scr_act());
  lv_obj_set_pos(startbtn, 10, 10);  /*Set its position*/
  lv_obj_set_size(startbtn, 80, 50); /*Set its size*/
  lv_obj_add_event_cb(startbtn, btn_event_Start, LV_EVENT_ALL, NULL);
  lv_obj_t *startbtnlabel = lv_label_create(startbtn); /*Add a label to the button*/
  lv_label_set_text(startbtnlabel, "Start");           /*Set the labels text*/
  lv_obj_center(startbtnlabel);

  lv_obj_t *btn = lv_btn_create(lv_scr_act());                   /*Add a button the current screen*/
  lv_obj_set_pos(btn, 100, 10);                                  /*Set its position*/
  lv_obj_set_size(btn, 80, 50);                                  /*Set its size*/
  lv_obj_add_event_cb(btn, btn_event_Pause, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/
  lv_obj_t *btnlabel = lv_label_create(btn);                     /*Add a label to the button*/
  lv_label_set_text(btnlabel, "Stop");                           /*Set the labels text*/
  lv_obj_center(btnlabel);

  const char *opts = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
  lv_obj_t *roller100 = lv_roller_create(lv_scr_act());
  lv_roller_set_options(roller100, opts, LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(roller100, 3);
  lv_obj_align(roller100, LV_ALIGN_LEFT_MID, 10, 0);
  lv_obj_add_event_cb(roller100, roller_event_UpdateRateHun, LV_EVENT_ALL, NULL);

  lv_obj_t *roller10 = lv_roller_create(lv_scr_act());
  lv_roller_set_options(roller10, opts, LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(roller10, 3);
  lv_obj_align_to(roller10, roller100, LV_ALIGN_RIGHT_MID, 50, 0);
  lv_obj_add_event_cb(roller10, roller_event_UpdateRateTen, LV_EVENT_ALL, NULL);

  lv_obj_t *roller1 = lv_roller_create(lv_scr_act());
  lv_roller_set_options(roller1, opts, LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(roller1, 3);
  lv_obj_align_to(roller1, roller10, LV_ALIGN_RIGHT_MID, 50, 0);
  lv_obj_add_event_cb(roller1, roller_event_UpdateRateOne, LV_EVENT_ALL, NULL);

  lv_obj_t *rollerunit = lv_roller_create(lv_scr_act());
  lv_roller_set_options(rollerunit, "ml/sec\nml/min\nml/h", LV_ROLLER_MODE_INFINITE);
  lv_roller_set_visible_row_count(rollerunit, 2);
  lv_obj_align(rollerunit, LV_ALIGN_RIGHT_MID, -10, 0);
  lv_obj_add_event_cb(rollerunit, roller_event_UpdateRateUnit, LV_EVENT_ALL, NULL);

  statusLabel = lv_label_create(lv_scr_act());
  lv_obj_set_pos(statusLabel, 50, 50);
  lv_obj_set_size(statusLabel, 80, 50);
  lv_label_set_text(statusLabel, "Paused");

  rateLabel = lv_label_create(lv_scr_act());
  lv_label_set_text(rateLabel, "Rate: 0 ml/sec");
  lv_obj_align(rateLabel, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_align_to(statusLabel, rateLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  // lv_obj_align(test_roller, LV_ALIGN_BOTTOM_MID,0,0);

  while (1)
  {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(5 * 1000);
  }

  hal_deinit();
  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
  /* mouse input device */
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1);
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  /* keyboard input device */
  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2);
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;

  /* mouse scroll wheel input device */
  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3);
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);

  lv_disp_t *disp = NULL;

#if USE_SDL
  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  sdl_init();

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[MONITOR_HOR_RES * 100];
  static lv_color_t buf1_2[MONITOR_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = MONITOR_HOR_RES;
  disp_drv.ver_res = MONITOR_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  // mouse_init();
  indev_drv_1.read_cb = sdl_mouse_read;

  // keyboard_init();
  indev_drv_2.read_cb = sdl_keyboard_read;

  // mousewheel_init();
  indev_drv_3.read_cb = sdl_mousewheel_read;

#elif USE_X11
  lv_x11_init("LVGL Simulator Demo", DISP_HOR_RES, DISP_VER_RES);

  /*Create a display buffer*/
  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[DISP_HOR_RES * 100];
  static lv_color_t buf1_2[DISP_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISP_HOR_RES * 100);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = lv_x11_flush;
  disp_drv.hor_res = DISP_HOR_RES;
  disp_drv.ver_res = DISP_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  /* Add the input device driver */
  indev_drv_1.read_cb = lv_x11_get_pointer;
  indev_drv_2.read_cb = lv_x11_get_keyboard;
  indev_drv_3.read_cb = lv_x11_get_mousewheel;
#endif

  /* Set diplay theme */
  lv_theme_t *th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  /* Tick init */
  end_tick = false;
  pthread_create(&thr_tick, NULL, tick_thread, NULL);

  /* register input devices */
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(kb_indev, g);
  lv_indev_set_group(enc_indev, g);

  /* Set a cursor for the mouse */
  LV_IMG_DECLARE(mouse_cursor_icon);                  /*Declare the image file.*/
  lv_obj_t *cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor*/
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);     /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor_obj);       /*Connect the image  object to the driver*/
}

/**
 * Releases the Hardware Abstraction Layer (HAL) for the LVGL graphics library
 */
static void hal_deinit(void)
{
  end_tick = true;
  pthread_join(thr_tick, NULL);

#if USE_SDL
  // nop
#elif USE_X11
  lv_x11_deinit();
#endif
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static void *tick_thread(void *data)
{
  (void)data;

  while (!end_tick)
  {
    usleep(5000);
    lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
  }

  return NULL;
}
