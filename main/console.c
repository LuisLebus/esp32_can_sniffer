/**
 * @file console.c
 * @author
 * @date
 * @brief
 */

//=============================================================================
// [Inclusions] ===============================================================
#include <errno.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"

#include "console.h"
#include "sniffer.h"

//=============================================================================

//=============================================================================
// [Private Defines] ==========================================================
#define CONSOLE_TAG     "[CONSOLE]"

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
static const char * prompt = LOG_COLOR_I ">>> " LOG_RESET_COLOR;

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
// [Local Function Declarations] ==============================================

/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
static void console_process_task(void * pvParameter);


/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
static int cmd_1(int argc, char ** argv);


/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
static int cmd_2(int argc, char ** argv);


/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
static int cmd_3(int argc, char ** argv);


/**
 * @details
 * @param[in]
 * @param[in]
 * @return
 */
static void register_cmd(void);

//=============================================================================

//=============================================================================
// [FreeRTOS Task Definitions] ================================================

static void console_process_task(void * pvParameter)
{
    int ret = 0;
    esp_err_t err = ESP_FAIL;

    vTaskDelay(pdMS_TO_TICKS(5000));

    while(1)
    {
        char * line = linenoise(prompt);

        if (NULL == line)
        {
            continue;
        }

        linenoiseHistoryAdd(line);

        err = esp_console_run(line, &ret);

        if (ESP_ERR_NOT_FOUND == err)
        {
            printf("Unrecognized command\n");
        }
        else if (ESP_ERR_INVALID_ARG == err)
        {
            // Command was empty.
        }
        else if ((ESP_OK == err) && (ret != ESP_OK))
        {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(err));
        }
        else if (err != ESP_OK)
        {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }
        else
        {
            // Nothing to do.
        }

        linenoiseFree(line);
    }
}
// End static void console_process_task(void * pvParameter)

//=============================================================================

//=============================================================================
// [Local Function Definitions] ===============================================

static int cmd_1(int argc, char ** argv)
{
    sniffer_cmd_1();

    return 0;
}
// End static int cmd_1(int argc, char ** argv)

static int cmd_2(int argc, char ** argv)
{
    sniffer_cmd_2();

    return 0;
}
// End static int cmd_2(int argc, char ** argv)

static int cmd_3(int argc, char ** argv)
{
    sniffer_cmd_3();

    return 0;
}
// End static int cmd_3(int argc, char ** argv)

static void register_cmd(void)
{
    esp_console_cmd_t cmd = {0};

    cmd.command = "1";
    cmd.help = "CMD 1";
    cmd.hint = NULL;
    cmd.func = &cmd_1;
    cmd.argtable = NULL;
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );

    cmd.command = "2";
    cmd.help = "CMD 2";
    cmd.hint = NULL;
    cmd.func = &cmd_2;
    cmd.argtable = NULL;
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );

    cmd.command = "3";
    cmd.help = "CMD 3";
    cmd.hint = NULL;
    cmd.func = &cmd_3;
    cmd.argtable = NULL;
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
// End static void register_cmd(void)

//=============================================================================

//=============================================================================
// [External Function Definition] =============================================

void console_init(void)
{
    /* Disable buffering on stdin */
    setvbuf(stdin, NULL, _IONBF, 0);

    /* Minicom, screen, idf_monitor send CR when ENTER key is pressed */
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

    /* Configure UART. Note that REF_TICK is used so that the baud rate remains
     * correct while APB frequency is changing in light sleep mode.
     */
    const uart_config_t uart_config = {
            .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .use_ref_tick = true
    };
    ESP_ERROR_CHECK( uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config) );

    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0) );

    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

    /* Initialize the console */
    esp_console_config_t console_config = {
            .max_cmdline_args = 8,
            .max_cmdline_length = 256,
#if CONFIG_LOG_COLORS
            .hint_color = atoi(LOG_COLOR_CYAN)
#endif
    };

    ESP_ERROR_CHECK( esp_console_init(&console_config) );

    /* Configure linenoise line completion library */
    /* Enable multiline editing. If not set, long commands will scroll within
     * single line.
     */
    linenoiseSetMultiLine(1);

    /* Tell linenoise where to get command completions and hints */
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback*) &esp_console_get_hint);

    /* Set command history size */
    linenoiseHistorySetMaxLen(100);

    /* Register commands */
    esp_console_register_help_command();
    register_cmd();

    /* Figure out if the terminal supports escape sequences */
    int probe_status = linenoiseProbe();

    if (probe_status)
    {
        printf("\n"
                "Your terminal application does not support escape sequences.\n"
                "Line editing and history features are disabled.\n"
                "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);

#if CONFIG_LOG_COLORS
        prompt = ">>> ";
#endif

    }

    xTaskCreate(console_process_task, "console_proc_ts", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1, NULL);
}
// End void console_init(void)

//=============================================================================

//====================== [End Document] =======================================
