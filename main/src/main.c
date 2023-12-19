
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

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if 0
static void user_image_demo()
{
  lv_obj_t * img = lv_gif_create(lv_scr_act());
  lv_gif_set_src(img, "A:lvgl/examples/libs/gif/bulb.gif");
  lv_obj_align(img, LV_ALIGN_BOTTOM_RIGHT, -20, -20);

  lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    lv_obj_t * qr = lv_qrcode_create(lv_scr_act(), 150, fg_color, bg_color);

    /*Set data*/
    const char * data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data));
    lv_obj_center(qr);

    /*Add a border with bg_color*/
    lv_obj_set_style_border_color(qr, bg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);

    /*Create a font*/
    static lv_ft_info_t info;
    /*FreeType uses C standard file system, so no driver letter is required.*/
    info.name = "./lvgl/examples/libs/freetype/Lato-Regular.ttf";
    info.weight = 24;
    info.style = FT_FONT_STYLE_NORMAL;
    info.mem = NULL;
    if(!lv_ft_font_init(&info)) {
        LV_LOG_ERROR("create failed.");
    }

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, info.font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_label_set_text(label, "Hello world\nI'm a font created with FreeType");
    lv_obj_set_pos(label, 10, 10);

    lv_obj_t *  img1 = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img1, "A:lvgl/examples/libs/png/wink.png");
    lv_obj_align(img1, LV_ALIGN_LEFT_MID, 20, 0);

    lv_obj_t * wp;

    wp = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(wp, "A:lvgl/examples/libs/sjpg/small_image.sjpg");
    lv_obj_align(wp, LV_ALIGN_RIGHT_MID, -20, 0);

    lv_obj_t * img2 = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
    lv_img_set_src(img2, "A:lvgl/examples/libs/sjpg/lv_example_jpg.jpg");
    //lv_obj_center(img);
    lv_obj_align(img2, LV_ALIGN_TOP_RIGHT, -20, 20);

    lv_obj_t * img3 = lv_img_create(lv_scr_act());
    /* Assuming a File system is attached to letter 'A'
     * E.g. set LV_USE_FS_STDIO 'A' in lv_conf.h */
#if LV_COLOR_DEPTH == 32
    lv_img_set_src(img3, "A:lvgl/examples/libs/bmp/example_32bit.bmp");
#elif LV_COLOR_DEPTH == 16
    lv_img_set_src(img, "A:lvgl/examples/libs/bmp/example_16bit.bmp");
#endif
    lv_obj_align(img3, LV_ALIGN_BOTTOM_MID, 0, -20);

    lv_obj_t * img4 = lv_img_create(lv_scr_act());
    lv_img_set_src(img4, "A:lvgl/examples/libs/ffmpeg/ffmpeg.png");
    lv_obj_align(img4, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    lv_obj_t * player = lv_ffmpeg_player_create(lv_scr_act());
    lv_ffmpeg_player_set_src(player, "./lvgl/examples/libs/ffmpeg/birds.mp4");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    lv_obj_align(player, LV_ALIGN_TOP_MID, 0, 20);
}
#endif



static lv_obj_t * rrlabel;
static lv_obj_t * sslabel;
static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Stop: %d", cnt);
        lv_label_set_text(sslabel, "It's rest.");


        
    }
}

static void btn_event_cb3(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Start: %d", cnt);
        lv_label_set_text(sslabel, "It's on!!");


        
    }
}
static void btn_event2_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text(rrlabel, lv_label_get_text(label));
    }
}




static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);

        if(x==0){
          lv_obj_t * label = lv_obj_get_child(child, 0);
          lv_label_set_text(rrlabel, lv_label_get_text(label));
        }
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

  //  lv_example_switch_1();
  //  lv_example_calendar_1();
  //  lv_example_btnmatrix_2();
  //  lv_example_checkbox_1();
  //  lv_example_colorwheel_1();
  //  lv_example_chart_6();
  //  lv_example_table_2();
  //  lv_example_scroll_2();
  //  lv_example_textarea_1();
  //  lv_example_msgbox_1();
  //  lv_example_dropdown_2();
  //  lv_example_btn_1();
  //  lv_example_scroll_1();
  //  lv_example_tabview_1();
  //  lv_example_tabview_1();
  //  lv_example_flex_3();
  //  lv_example_label_1();

  // lv_demo_widgets();
  //  lv_demo_keypad_encoder();
  //  lv_demo_benchmark();
  //  lv_demo_stress();
  //  lv_demo_music();

  // lv_obj_t *label = lv_label_create(lv_scr_act());
  // lv_label_set_text(label, "Hello Ardino and LVGL!");
  // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t * startbtn = lv_btn_create(lv_scr_act()); 
    lv_obj_set_pos(startbtn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(startbtn, 80, 50);                          /*Set its size*/
    lv_obj_add_event_cb(startbtn, btn_event_cb3, LV_EVENT_ALL, NULL);
    lv_obj_t * startbtnlabel = lv_label_create(startbtn);          /*Add a label to the button*/
    lv_label_set_text(startbtnlabel, "Start");                     /*Set the labels text*/
    lv_obj_center(startbtnlabel);

    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 100, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 80, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/
    lv_obj_t * btnlabel = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(btnlabel, "Stop");                     /*Set the labels text*/
    lv_obj_center(btnlabel);
    
    
    lv_obj_set_pos(sslabel,50,50);
    lv_obj_set_size(sslabel,80,50); 
    lv_label_set_text(sslabel, "Stages"); 



    

    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 200, 200);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    uint32_t i;
    for(i = 0; i < 10; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_width(btn, lv_pct(100));
        lv_obj_add_event_cb(btn, btn_event2_cb, LV_EVENT_ALL, NULL);
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Rate %"LV_PRIu32, i);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_OFF);

    rrlabel = lv_label_create(lv_scr_act());
    lv_label_set_text(rrlabel, "Rate: 0");
    lv_obj_align_to(rrlabel, cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 50);




  //  user_image_demo();

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
