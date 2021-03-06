/* Jolt Wallet - Open Source Cryptocurrency Hardware Wallet
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 */


#ifndef __INCLUDE_WIFI_H__
#define __INCLUDE_WIFI_H__

// The identity of the access point to which we wish to connect.
//#define AP_TARGET_SSID     ""

// The password we need to supply to the access point for authorization.
//#define AP_TARGET_PASSWORD ""

void wifi_connect();
void get_ap_info(char * ssid_info, size_t size);
void wifi_task(void *pvParameters);
uint8_t get_wifi_strength();

#endif
