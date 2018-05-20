#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sodium.h"
#include <string.h>
#include "esp_log.h"

#include "nano_lib.h"

#include "menu8g2.h"
#include "../../../vault.h"
#include "submenus.h"
#include "../../../globals.h"
#include "../../../loading.h"
#include "../../../gui.h"

#include "nano_lws.h"
#include "nano_parse.h"


static const char TAG[] = "nano_receive";
static const char TITLE[] = "Receive Nano";

void menu_nano_receive(menu8g2_t *prev){
    /*
     * Blocks involved:
     * pending_block - The send we want to sign in
     * new_block - block we are creating
     * prev_block - frontier of our account chain
     */

    vault_rpc_t rpc;
    menu8g2_t menu;
    menu8g2_copy(&menu, prev);
    
    /******************
     * Get My Address *
     ******************/
    nvs_handle nvs_h;
    init_nvm_namespace(&nvs_h, "nano");
    if(ESP_OK != nvs_get_u32(nvs_h, "index", &(rpc.nano_public_key.index))){
        rpc.nano_public_key.index = 0;
    }
    nvs_close(nvs_h);

    sodium_memzero(&rpc, sizeof(rpc));
    rpc.type = NANO_PUBLIC_KEY;
    if(vault_rpc(&rpc) != RPC_SUCCESS){
        return;
    }
    uint256_t my_public_key;
    memcpy(my_public_key, rpc.nano_public_key.block.account, sizeof(my_public_key));

    char my_address[ADDRESS_BUF_LEN];
    nl_public_to_address(my_address, sizeof(my_address), my_public_key);
    
    ESP_LOGI(TAG, "My Address: %s\n", my_address);

    /*********************
     * Get Pending Block *
     *********************/
    // Outcome: 
    //     * pending_hash, pending_amount
    // Returns if no pending blocks. Pending_amount doesn't need to be verified
    // since nothing malicious can be done with a wrong pending_amount.
    loading_enable();
    loading_text_title("Checking Pending", TITLE);
    hex256_t pending_hash;
    
    /* Search for pending block(s) */
    mbedtls_mpi transaction_amount;
    mbedtls_mpi_init(&transaction_amount);
    if (get_pending(my_address, pending_hash, &transaction_amount) != E_SUCCESS) {
        loading_disable();
        menu8g2_display_text_title(&menu, "No Pending Blocks Found", TITLE);
        goto exit;
    }

    ESP_LOGI(TAG, "Pending Hash: %s", pending_hash);
    #if LOG_LOCAL_LEVEL >= ESP_LOG_INFO
    {
    //char amount[BALANCE_DEC_BUF_LEN];
    char amount[66];
    size_t olen;
    if(mbedtls_mpi_write_string(&transaction_amount, 10, amount, sizeof(amount), &olen)){
        ESP_LOGE(TAG, "Unable to write string from mbedtls_mpi; olen: %d", olen);
    }
    ESP_LOGI(TAG, "Pending Amount: %s", amount);
    }
    #endif

    /********************************************
     * Get My Account's Frontier Block and Work *
     ********************************************/
    // Assumes State Blocks Only
    // Outcome:
    //     * frontier_hash, frontier_block
    hex256_t frontier_hash = { 0 };
    nl_block_t frontier_block;
    nl_block_init(&frontier_block);
    uint64_t proof_of_work;

    loading_text_title("Checking Account Frontier", TITLE);
    if( get_frontier(my_address, frontier_hash) == E_SUCCESS ){
        ESP_LOGI(TAG, "Creating RECEIVE Block");

        if( get_block(frontier_hash, &frontier_block) != E_SUCCESS ){
            ESP_LOGI(TAG, "Error retrieving frontier block.");
            loading_disable();
            goto exit;
        }

        // Get RECEIVE work
        loading_text_title("Fetching Work", TITLE);
        if( E_SUCCESS != get_work( frontier_hash, &proof_of_work ) ){
            ESP_LOGI(TAG, "Invalid Work (RECEIVE) Response.");
            loading_disable();
            goto exit;
        }

    }
    else {
        // Get OPEN work
        ESP_LOGI(TAG, "Creating OPEN Block");
        hex256_t work_hex;
        sodium_bin2hex(work_hex, sizeof(work_hex), my_public_key, sizeof(my_public_key));
        loading_text_title("Fetching Work", TITLE);
        if( E_SUCCESS != get_work( work_hex, &proof_of_work ) ){
            ESP_LOGI(TAG, "Invalid Work (OPEN) Response.");
            loading_disable();
            goto exit;
        }
    }

    /********************************
     * Create open or receive block *
     ********************************/
    loading_text_title("Creating Receive", TITLE);
    sodium_memzero(&rpc, sizeof(rpc));
    rpc.type = NANO_BLOCK_SIGN;
    nl_block_t *new_block = &(rpc.nano_block_sign.block);

    new_block->type = STATE;
    sodium_hex2bin(new_block->previous, sizeof(new_block->previous),
            frontier_hash, sizeof(frontier_hash), NULL, NULL, NULL);
    memcpy(new_block->account, my_public_key, sizeof(my_public_key));
    nl_address_to_public(new_block->representative, my_address); //todo: default rep
    sodium_hex2bin(new_block->link, sizeof(new_block->link),
            pending_hash, sizeof(pending_hash), NULL, NULL, NULL);
    // Unsigned addition so this could never accidentilly become a send
    mbedtls_mpi_add_abs(&(new_block->balance), &transaction_amount, &(frontier_block.balance));
    new_block->work = proof_of_work;

    #if LOG_LOCAL_LEVEL >= ESP_LOG_INFO
    {
    char amount[66];
    size_t olen;
    mbedtls_mpi_write_string(&(frontier_block.balance), 10, amount, sizeof(amount), &olen);
    ESP_LOGI(TAG, "Frontier Amount: %s", amount);
    mbedtls_mpi_write_string(&(new_block->balance), 10, amount, sizeof(amount), &olen);
    ESP_LOGI(TAG, "New Block Amount: %s", amount);
    mbedtls_mpi_write_string(&transaction_amount, 10, amount, sizeof(amount), &olen);
    ESP_LOGI(TAG, "Transaction Amount: %s", amount);
    }
    #endif

    // Sign block
    loading_text_title("Signing Receive", TITLE);
    if(vault_rpc(&rpc) != RPC_SUCCESS){
        goto exit;
    }
    
    loading_text_title("Broadcasting Transaction", TITLE);
    process_block(new_block);
    
    loading_disable();
    menu8g2_display_text_title(&menu, "Blocks Processed", TITLE);

    exit:
        loading_disable();
        return;
}