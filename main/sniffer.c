/**
 * @file sniffer.c
 * @author
 * @date
 * @brief
 */

//=============================================================================
// [Inclusions] ===============================================================
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/twai.h"
#include "esp_log.h"

#include "sniffer.h"
#include "can.h"

//=============================================================================

//=============================================================================
// [Private Defines] ==========================================================
#define SNIFFER_TAG     "[SNIFFER]"

#define ID_DEF        	0x60
#define ID_1          	0x61
#define ID_BROADCAST  	0xFF
#define ID_MASTER     	0x01

//=============================================================================

//=============================================================================
// [Local Typedef] ============================================================

//=============================================================================

//=============================================================================
// [External Data Definitions] ================================================

// Const ---------------------------------------------
//----------------------------------------------------

// Vars ----------------------------------------------
//----------------------------------------------------

// Task Handlers -------------------------------------
//----------------------------------------------------

// Queue Handlers ------------------------------------
//----------------------------------------------------

// Event Group Handlers ------------------------------
//----------------------------------------------------

// Semaphore Handlers --------------------------------
//----------------------------------------------------

//=============================================================================

//=============================================================================
// [Local Data Declarations] ==================================================

// Const ---------------------------------------------
//----------------------------------------------------

// Vars ----------------------------------------------
static uint8_t sourse_addr_only = ID_DEF;
static bool sourse_addr_all = true;

//----------------------------------------------------

// Task Handlers -------------------------------------
//----------------------------------------------------

// Queue Handlers ------------------------------------
//----------------------------------------------------

// Event Group Handlers ------------------------------
//----------------------------------------------------

// Semaphore Handlers --------------------------------
//----------------------------------------------------

//=============================================================================

//=============================================================================
// [Local Function Declarations] ==============================================

/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
static void sniffer_process_task(void * pvParameter);

//=============================================================================

//=============================================================================
// [FreeRTOS Task Definitions] ================================================

static void sniffer_process_task(void * pvParameter)
{
    twai_message_t twai_message = {0};
    uint8_t sourse_addr = 0;
    uint8_t destination_addr = 0;
    uint8_t msg_id = 0;
    uint8_t data_page = 0;
    uint8_t priority = 0;

    while (1)
    {
        if (true == can_read(&twai_message, portMAX_DELAY))
        {
            sourse_addr = (uint8_t)(twai_message.identifier);
            destination_addr = (uint8_t)(twai_message.identifier >> 8);
            msg_id = (uint8_t)(twai_message.identifier >> 16);
            data_page = ((uint8_t)(twai_message.identifier >> 24)) & 0b00000001;
            priority = ((uint8_t)(twai_message.identifier >> 26)) & 0b00000111;


            if (true == sourse_addr_all)
            {
                if (ID_MASTER == sourse_addr)
                {
                    ESP_LOGE(SNIFFER_TAG, "So:%02x De:%02x Id:%02x         Dp:%02x Pr:%02x Co:%u         %02x %02x %02x %02x %02x %02x %02x %02x",
                            sourse_addr,
                            destination_addr,
                            msg_id,
                            data_page,
                            priority,
                            twai_message.data_length_code,
                            twai_message.data[7],
                            twai_message.data[6],
                            twai_message.data[5],
                            twai_message.data[4],
                            twai_message.data[3],
                            twai_message.data[2],
                            twai_message.data[1],
                            twai_message.data[0]);
                }
                else
                {
                    ESP_LOGI(SNIFFER_TAG, "So:%02x De:%02x Id:%02x         Dp:%02x Pr:%02x Co:%u         %02x %02x %02x %02x %02x %02x %02x %02x",
                            sourse_addr,
                            destination_addr,
                            msg_id,
                            data_page,
                            priority,
                            twai_message.data_length_code,
                            twai_message.data[7],
                            twai_message.data[6],
                            twai_message.data[5],
                            twai_message.data[4],
                            twai_message.data[3],
                            twai_message.data[2],
                            twai_message.data[1],
                            twai_message.data[0]);
                }
            }
            else
            {
                if (sourse_addr_only == sourse_addr)
                {
                        ESP_LOGW(SNIFFER_TAG, "So:%02x De:%02x Id:%02x         Dp:%02x Pr:%02x Co:%u         %02x %02x %02x %02x %02x %02x %02x %02x",
                                sourse_addr,
                                destination_addr,
                                msg_id,
                                data_page,
                                priority,
                                twai_message.data_length_code,
                                twai_message.data[7],
                                twai_message.data[6],
                                twai_message.data[5],
                                twai_message.data[4],
                                twai_message.data[3],
                                twai_message.data[2],
                                twai_message.data[1],
                                twai_message.data[0]);
                }
            }
        }
    }
}
// End static void sniffer_process_task(void * pvParameter)

//=============================================================================

//=============================================================================
// [Local Function Definitions] ===============================================

//=============================================================================

//=============================================================================
// [External Function Definition] =============================================

void sniffer_init(void)
{
    can_init();

    xTaskCreate(sniffer_process_task, "sniffer_proc_ts", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1, NULL);
}
// End void sniffer_init(void)


void sniffer_cmd_1(void)
{
    sourse_addr_only = ID_MASTER;
    ESP_LOGI(SNIFFER_TAG, "ID MASTER");
}
// End void sniffer_cmd_1(void)


void sniffer_cmd_2(void)
{
    sourse_addr_only = ID_1;
    ESP_LOGI(SNIFFER_TAG, "ID 1");
}
// End void sniffer_cmd_2(void)


void sniffer_cmd_3(void)
{
    if (true == sourse_addr_all)
    {
        sourse_addr_all = false;
        ESP_LOGI(SNIFFER_TAG, "ID ALL FALSE");
    }
    else
    {
        sourse_addr_all = true;
        ESP_LOGI(SNIFFER_TAG, "ID ALL TRUE");
    }
}
// End void sniffer_cmd_3(void)


//=============================================================================

//====================== [End Document] =======================================
