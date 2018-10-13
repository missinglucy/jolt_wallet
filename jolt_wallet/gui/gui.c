/* Jolt Wallet - Open Source Cryptocurrency Hardware Wallet
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 */


#include "u8g2.h"
#include "menu8g2.h"
#include "nvs.h"
#include "esp_log.h"

#include "graphics.h"
#include "gui.h"
#include "statusbar.h"
#include "loading.h"
#include "../vault.h"
#include "../helpers.h"
#include "../globals.h"
#include "../hal/u8g2_esp32.h"
#include "../hal/storage.h"
#include "../syscore/filesystem.h"
#include "../syscore/launcher.h"

#include "menus/submenus.h"

static const char TAG[] = "GUI";


void setup_screen(u8g2_t *u8g2){
    // Initialize OLED Screen I2C params
    // Already assumes i2c drivers have been installed
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal_init(u8g2_esp32_hal);
   
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        u8g2,
        U8G2_R0,
        u8g2_esp32_i2c_byte_cb,
        u8g2_esp32_gpio_and_delay_cb
    );  // init u8g2 structure

    // Note: SCREEN_ADDRESS is already shifted left by 1
    u8x8_SetI2CAddress(&(u8g2->u8x8), CONFIG_JOLT_DISPLAY_ADDRESS);

    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0); // wake up display
    u8g2_ClearDisplay(u8g2);
    u8g2_ClearBuffer(u8g2);

#if CONFIG_JOLT_DISPLAY_FLIP
    u8g2_SetFlipMode(u8g2, true);
#endif
    u8g2_SetContrast(u8g2, 255);
}

static char **fns; // Used to store user elf filenames

static void launch_file_proxy(menu8g2_t *prev) {
    launch_file(fns[prev->index], "app_main", 0, NULL);
}

void gui_task(){
    /* Master GUI Task */
    uint8_t boot_splash_enable;

    // display boot_splash if option is set
    storage_get_u8(&boot_splash_enable, "user", "boot_splash",
            (uint8_t)CONFIG_JOLT_DEFAULT_BOOT_SPLASH_ENABLE);
    if(boot_splash_enable){
        boot_splash();
    }

    xTaskCreate(statusbar_task, "StatusBarTask", 8192,
            NULL, 19, NULL);
    loading_init();

    const char title[] = "Main";

    /* MAIN MENU CONSTRUCTION */
    // Find and Verify All User Apps
    uint32_t n_fns = get_all_fns(NULL, 0, ".elf", true);
    ESP_LOGD(TAG, "Found %x apps.", n_fns);
    if( n_fns > 0 ) {
        fns = malloc_char_array(n_fns);
        get_all_fns(fns, n_fns, ".elf", true);
    }

    menu8g2_elements_t elements;
    menu8g2_elements_init(&elements, 2 + n_fns);
    for(uint8_t i=0; i<n_fns; i++) {
        ESP_LOGD(TAG, "Registering App \"%s\" into the GUI", fns[i]);
        menu8g2_set_element(&elements, fns[i], &launch_file_proxy);
    }
    menu8g2_set_element(&elements, "Console", &menu_console);
    menu8g2_set_element(&elements, "Settings", &menu_settings);

    // Main GUI Menu Loop
    for(;;) {
        menu8g2_create_vertical_element_menu(menu, title, &elements);
        menu->index = 0;
    }

    // Should never reach here!
    free_char_array(fns, n_fns);
    menu8g2_elements_free(&elements);
    vTaskDelete(NULL);
}