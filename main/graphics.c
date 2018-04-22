#include "graphics.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"


const unsigned char graphic_nano_logo_small[] = {
  0x00, 0x00, 0x06, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 
  0xF0, 0x01, 0x00, 0x80, 0x1F, 0x00, 0x00, 0xF8, 0x03, 0x00, 0x80, 0x3F, 
  0x00, 0x00, 0xF8, 0x03, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0xF8, 0x03, 0x00, 
  0xC0, 0x3F, 0x00, 0x00, 0xFC, 0x01, 0x00, 0xE0, 0x7F, 0x00, 0x00, 0xFC, 
  0x00, 0x00, 0xF8, 0xF0, 0x01, 0x80, 0x1F, 0x00, 0x00, 0x7E, 0xE0, 0x07, 
  0xC0, 0x07, 0x00, 0x00, 0x7F, 0xC0, 0x0F, 0xE0, 0x07, 0x00, 0x00, 0x7F, 
  0xC0, 0x0F, 0xF0, 0x07, 0x00, 0x00, 0x3F, 0xC0, 0x0F, 0xF0, 0x07, 0x00, 
  0x00, 0x3F, 0x80, 0x0F, 0xF0, 0x03, 0x00, 0x00, 0x1E, 0x00, 0x07, 0xF8, 
  0x01, 0x00, 0x18, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3C, 0x00, 0x00, 
  0xC0, 0x1F, 0x00, 0x00, 0x7E, 0x00, 0x00, 0xE0, 0x1F, 0x00, 0x00, 0xFF, 
  0x00, 0x00, 0xE0, 0x0F, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xE0, 0x0F, 0x00, 
  0x00, 0x7E, 0x00, 0x00, 0xC0, 0x07, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x80, 
  0x03, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const unsigned char graphic_nano_ray[] = {
  0x00, 0x00, 0x80, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x0F, 0x00, 0x3E, 
  0x70, 0xE0, 0x00, 0x38, 0xFF, 0x1F, 0x00, 0xFC, 0x3F, 0xE0, 0x00, 0x38, 
  0x03, 0x38, 0x00, 0xF8, 0x3F, 0xC1, 0x01, 0x1C, 0x03, 0x70, 0x00, 0xF0, 
  0xFF, 0xC1, 0x01, 0x1C, 0x03, 0x60, 0x00, 0xF0, 0xFF, 0x81, 0x03, 0x0E, 
  0x03, 0x60, 0x00, 0xE0, 0xFF, 0x00, 0x03, 0x06, 0x03, 0x60, 0x00, 0xE0, 
  0xFF, 0x00, 0x06, 0x03, 0x03, 0x60, 0x00, 0xE0, 0xFF, 0x00, 0x8E, 0x03, 
  0x03, 0x70, 0x00, 0xE0, 0xFF, 0x00, 0xFC, 0x01, 0x03, 0x38, 0x00, 0xE0, 
  0xFF, 0x00, 0xF8, 0x00, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0x00, 0x70, 0x00, 
  0xFF, 0x07, 0x00, 0xF0, 0xFF, 0x01, 0x70, 0x00, 0x03, 0x06, 0x00, 0x70, 
  0xF0, 0x01, 0x70, 0x00, 0x03, 0x0C, 0x00, 0x18, 0x80, 0x03, 0x70, 0x00, 
  0x03, 0x1C, 0x00, 0x0C, 0x80, 0x03, 0x70, 0x00, 0x03, 0x18, 0x00, 0x0E, 
  0x00, 0x03, 0x70, 0x00, 0x03, 0x38, 0x00, 0x07, 0x00, 0x02, 0x70, 0x00, 
  0x03, 0x30, 0x80, 0x01, 0x00, 0x02, 0x70, 0x00, 0x03, 0x60, 0xC0, 0x00, 
  0x00, 0x02, 0x70, 0x00, 0x03, 0x60, 0x60, 0x00, 0x00, 0x00, 0x70, 0x00, 
  0x03, 0xC0, 0x30, 0x00, 0x00, 0x00, 0x70, 0x00, };

void boot_splash(u8g2_t *u8g2){
    int16_t gap = 18;
    int16_t n_steps = 25;
    int16_t logo_start_x = (u8g2_GetDisplayWidth(u8g2) - 
            GRAPHIC_NANO_LOGO_SMALL_W) / 2;
    int16_t logo_y = (u8g2_GetDisplayHeight(u8g2)-GRAPHIC_NANO_LOGO_SMALL_H)/2;
    int16_t ray_y = (u8g2_GetDisplayHeight(u8g2)-GRAPHIC_NANO_RAY_H)/2;

	u8g2_FirstPage(u8g2);
	do{
		u8g2_DrawXBM( u8g2, logo_start_x, logo_y,
                GRAPHIC_NANO_LOGO_SMALL_W,
                GRAPHIC_NANO_LOGO_SMALL_H,
                graphic_nano_logo_small);
	} while(u8g2_NextPage(u8g2));
    vTaskDelay(1500 / portTICK_PERIOD_MS);

    // animation portion
    int16_t logo_final_x = (u8g2_GetDisplayWidth(u8g2) - 
            GRAPHIC_NANO_LOGO_SMALL_W - GRAPHIC_NANO_RAY_W - gap) / 2;
    int16_t ray_final_x = logo_final_x + GRAPHIC_NANO_LOGO_SMALL_W + gap ;
    int16_t ray_start_x = (u8g2_GetDisplayWidth(u8g2)/2) - GRAPHIC_NANO_RAY_W;
    for(int i=0; i < n_steps; i++){
	    u8g2_FirstPage(u8g2);
        do{
            // Draw RAY
			u8g2_SetDrawColor(u8g2, 1);
			u8g2_DrawXBM( u8g2,
					ray_start_x + i*(ray_final_x - ray_start_x)/n_steps,
					ray_y,
					GRAPHIC_NANO_RAY_W,
					GRAPHIC_NANO_RAY_H,
					graphic_nano_ray);
			// Blank Left Half of Screen
			u8g2_SetDrawColor(u8g2, 0);
			u8g2_DrawBox(u8g2, 0, 0,
					u8g2_GetDisplayWidth(u8g2)/2,
					u8g2_GetDisplayHeight(u8g2));
			// Draw Nano Logo
			u8g2_SetDrawColor(u8g2, 1);
			u8g2_DrawXBM( u8g2,
					logo_start_x + i*(logo_final_x - logo_start_x)/n_steps,
					ray_y,
					GRAPHIC_NANO_LOGO_SMALL_W,
					GRAPHIC_NANO_LOGO_SMALL_H,
					graphic_nano_logo_small);
        } while(u8g2_NextPage(u8g2));
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}
