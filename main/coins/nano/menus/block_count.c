#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "menu8g2.h"
#include "submenus.h"
#include "../../../globals.h"
#include "../../../statusbar.h"
#include "../../../gui.h"

#include <libwebsockets.h>
#include "nano_lws.h"
#include "nano_parse.h"

static const char TITLE[] = "Block Count";

void menu_nano_block_count(menu8g2_t *prev){
    char block_count[12];
    sprintf(block_count, "%d", get_block_count());
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);

    for(;;){
        if(menu8g2_display_text_title(&menu, block_count, TITLE) 
                == (1ULL << EASY_INPUT_BACK)){
            goto exit;
        }
    }

    exit:
        return;
}
