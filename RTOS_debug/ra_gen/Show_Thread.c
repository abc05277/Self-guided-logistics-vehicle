/* generated thread source file - do not edit */
#include "Show_Thread.h"

#if 1
                static StaticTask_t Show_Thread_memory;
                #if defined(__ARMCC_VERSION)           /* AC6 compiler */
                static uint8_t Show_Thread_stack[2048] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #else
                static uint8_t Show_Thread_stack[2048] BSP_PLACE_IN_SECTION(BSP_UNINIT_SECTION_PREFIX ".stack.Show_Thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
                #endif
                #endif
                TaskHandle_t Show_Thread;
                void Show_Thread_create(void);
                static void Show_Thread_func(void * pvParameters);
                void rtos_startup_err_callback(void * p_instance, void * p_data);
                void rtos_startup_common_init(void);
sci_uart_instance_ctrl_t     g_uart8_ctrl;

            baud_setting_t               g_uart8_baud_setting =
            {
                /* Baud rate calculated with 4.334% error. */ .semr_baudrate_bits_b.abcse = 0, .semr_baudrate_bits_b.abcs = 0, .semr_baudrate_bits_b.bgdm = 1, .cks = 0, .brr = 12, .mddr = (uint8_t) 256, .semr_baudrate_bits_b.brme = false
            };

            /** UART extended configuration for UARTonSCI HAL driver */
            const sci_uart_extended_cfg_t g_uart8_cfg_extend =
            {
                .clock                = SCI_UART_CLOCK_INT,
                .rx_edge_start          = SCI_UART_START_BIT_FALLING_EDGE,
                .noise_cancel         = SCI_UART_NOISE_CANCELLATION_DISABLE,
                .rx_fifo_trigger        = SCI_UART_RX_FIFO_TRIGGER_MAX,
                .p_baud_setting         = &g_uart8_baud_setting,
                .flow_control           = SCI_UART_FLOW_CONTROL_RTS,
                #if 0xFF != 0xFF
                .flow_control_pin       = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                .flow_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                .rs485_setting = {
                    .enable = SCI_UART_RS485_DISABLE,
                    .polarity = SCI_UART_RS485_DE_POLARITY_HIGH,
                #if 0xFF != 0xFF
                    .de_control_pin = BSP_IO_PORT_FF_PIN_0xFF,
                #else
                    .de_control_pin       = (bsp_io_port_pin_t) UINT16_MAX,
                #endif
                },
                .irda_setting = {
                    .ircr_bits_b.ire = 0,
                    .ircr_bits_b.irrxinv = 0,
                    .ircr_bits_b.irtxinv = 0,
                },
            };

            /** UART interface configuration */
            const uart_cfg_t g_uart8_cfg =
            {
                .channel             = 8,
                .data_bits           = UART_DATA_BITS_8,
                .parity              = UART_PARITY_OFF,
                .stop_bits           = UART_STOP_BITS_1,
                .p_callback          = uart8_callback,
                .p_context           = NULL,
                .p_extend            = &g_uart8_cfg_extend,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
                .rxi_ipl             = (12),
                .txi_ipl             = (12),
                .tei_ipl             = (12),
                .eri_ipl             = (12),
#if defined(VECTOR_NUMBER_SCI8_RXI)
                .rxi_irq             = VECTOR_NUMBER_SCI8_RXI,
#else
                .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI8_TXI)
                .txi_irq             = VECTOR_NUMBER_SCI8_TXI,
#else
                .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI8_TEI)
                .tei_irq             = VECTOR_NUMBER_SCI8_TEI,
#else
                .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI8_ERI)
                .eri_irq             = VECTOR_NUMBER_SCI8_ERI,
#else
                .eri_irq             = FSP_INVALID_VECTOR,
#endif
            };

/* Instance structure to use this module. */
const uart_instance_t g_uart8 =
{
    .p_ctrl        = &g_uart8_ctrl,
    .p_cfg         = &g_uart8_cfg,
    .p_api         = &g_uart_on_sci
};
#include "r_sci_i2c_cfg.h"
sci_i2c_instance_ctrl_t g_i2c4_ctrl;
const sci_i2c_extended_cfg_t g_i2c4_cfg_extend =
{
    /* Actual calculated bitrate: 99981. Actual SDA delay: 300 ns. */ .clock_settings.clk_divisor_value = 0, .clock_settings.brr_value = 20, .clock_settings.mddr_value = 172, .clock_settings.bitrate_modulation = true, .clock_settings.cycles_value = 30,
    .clock_settings.snfr_value         = (1),
};

const i2c_master_cfg_t g_i2c4_cfg =
{
    .channel             = 4,
    .rate                = I2C_MASTER_RATE_STANDARD,
    .slave               = 0x50,
    .addr_mode           = I2C_MASTER_ADDR_MODE_7BIT,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    .p_transfer_tx       = NULL,
#else
    .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    .p_transfer_rx       = NULL,
#else
    .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
    .p_callback          = sci_i2c4_master_callback,
    .p_context           = NULL,
#if defined(VECTOR_NUMBER_SCI4_RXI) && SCI_I2C_CFG_DTC_ENABLE
    .rxi_irq             = VECTOR_NUMBER_SCI4_RXI,
#else
    .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI4_TXI)
    .txi_irq             = VECTOR_NUMBER_SCI4_TXI,
#else
    .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_SCI4_TEI)
    .tei_irq             = VECTOR_NUMBER_SCI4_TEI,
#else
    .tei_irq             = FSP_INVALID_VECTOR,
#endif
    .ipl                 = (12),    /* (BSP_IRQ_DISABLED) is unused */
    .p_extend            = &g_i2c4_cfg_extend,
};
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c4 =
{
    .p_ctrl        = &g_i2c4_ctrl,
    .p_cfg         = &g_i2c4_cfg,
    .p_api         = &g_i2c_master_on_sci
};
iic_master_instance_ctrl_t g_i2c_1_ctrl;
const iic_master_extended_cfg_t g_i2c_1_extend =
{
    .timeout_mode             = IIC_MASTER_TIMEOUT_MODE_SHORT,
    .timeout_scl_low          = IIC_MASTER_TIMEOUT_SCL_LOW_ENABLED,
    .smbus_operation         = 0,
    /* Actual calculated bitrate: 1000000. Actual calculated duty cycle: 50%. */ .clock_settings.brl_value = 15, .clock_settings.brh_value = 15, .clock_settings.cks_value = 0, .clock_settings.sddl_value = 0, .clock_settings.dlcs_value = 0,
};
const i2c_master_cfg_t g_i2c_1_cfg =
{
    .channel             = 1,
    .rate                = I2C_MASTER_RATE_FASTPLUS,
    .slave               = 0x3C,
    .addr_mode           = I2C_MASTER_ADDR_MODE_7BIT,
#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_tx       = NULL,
#else
                .p_transfer_tx       = &RA_NOT_DEFINED,
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
                .p_transfer_rx       = NULL,
#else
                .p_transfer_rx       = &RA_NOT_DEFINED,
#endif
#undef RA_NOT_DEFINED
    .p_callback          = i2c2_callback,
    .p_context           = NULL,
#if defined(VECTOR_NUMBER_IIC1_RXI)
    .rxi_irq             = VECTOR_NUMBER_IIC1_RXI,
#else
    .rxi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC1_TXI)
    .txi_irq             = VECTOR_NUMBER_IIC1_TXI,
#else
    .txi_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC1_TEI)
    .tei_irq             = VECTOR_NUMBER_IIC1_TEI,
#else
    .tei_irq             = FSP_INVALID_VECTOR,
#endif
#if defined(VECTOR_NUMBER_IIC1_ERI)
    .eri_irq             = VECTOR_NUMBER_IIC1_ERI,
#else
    .eri_irq             = FSP_INVALID_VECTOR,
#endif
    .ipl                 = (11),
    .p_extend            = &g_i2c_1_extend,
};
/* Instance structure to use this module. */
const i2c_master_instance_t g_i2c_1 =
{
    .p_ctrl        = &g_i2c_1_ctrl,
    .p_cfg         = &g_i2c_1_cfg,
    .p_api         = &g_i2c_master_on_iic
};
extern uint32_t g_fsp_common_thread_count;

                const rm_freertos_port_parameters_t Show_Thread_parameters =
                {
                    .p_context = (void *) NULL,
                };

                void Show_Thread_create (void)
                {
                    /* Increment count so we will know the number of threads created in the RA Configuration editor. */
                    g_fsp_common_thread_count++;

                    /* Initialize each kernel object. */
                    

                    #if 1
                    Show_Thread = xTaskCreateStatic(
                    #else
                    BaseType_t Show_Thread_create_err = xTaskCreate(
                    #endif
                        Show_Thread_func,
                        (const char *)"Show_Thread",
                        2048/4, // In words, not bytes
                        (void *) &Show_Thread_parameters, //pvParameters
                        2,
                        #if 1
                        (StackType_t *)&Show_Thread_stack,
                        (StaticTask_t *)&Show_Thread_memory
                        #else
                        & Show_Thread
                        #endif
                    );

                    #if 1
                    if (NULL == Show_Thread)
                    {
                        rtos_startup_err_callback(Show_Thread, 0);
                    }
                    #else
                    if (pdPASS != Show_Thread_create_err)
                    {
                        rtos_startup_err_callback(Show_Thread, 0);
                    }
                    #endif
                }
                static void Show_Thread_func (void * pvParameters)
                {
                    /* Initialize common components */
                    rtos_startup_common_init();

                    /* Initialize each module instance. */
                    

                    #if (1 == BSP_TZ_NONSECURE_BUILD) && (1 == 1)
                    /* When FreeRTOS is used in a non-secure TrustZone application, portALLOCATE_SECURE_CONTEXT must be called prior
                     * to calling any non-secure callable function in a thread. The parameter is unused in the FSP implementation.
                     * If no slots are available then configASSERT() will be called from vPortSVCHandler_C(). If this occurs, the
                     * application will need to either increase the value of the "Process Stack Slots" Property in the rm_tz_context
                     * module in the secure project or decrease the number of threads in the non-secure project that are allocating
                     * a secure context. Users can control which threads allocate a secure context via the Properties tab when
                     * selecting each thread. Note that the idle thread in FreeRTOS requires a secure context so the application
                     * will need at least 1 secure context even if no user threads make secure calls. */
                     portALLOCATE_SECURE_CONTEXT(0);
                    #endif

                    /* Enter user code for this thread. Pass task handle. */
                    Show_Thread_entry(pvParameters);
                }
