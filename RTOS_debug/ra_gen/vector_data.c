/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI7 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI7 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI7 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI7 ERI (Receive error) */
            [4] = iic_master_rxi_isr, /* IIC1 RXI (Receive data full) */
            [5] = iic_master_txi_isr, /* IIC1 TXI (Transmit data empty) */
            [6] = iic_master_tei_isr, /* IIC1 TEI (Transmit end) */
            [7] = iic_master_eri_isr, /* IIC1 ERI (Transfer error) */
            [8] = sci_i2c_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [9] = sci_i2c_tei_isr, /* SCI4 TEI (Transmit end) */
            [10] = sci_uart_rxi_isr, /* SCI8 RXI (Receive data full) */
            [11] = sci_uart_txi_isr, /* SCI8 TXI (Transmit data empty) */
            [12] = sci_uart_tei_isr, /* SCI8 TEI (Transmit end) */
            [13] = sci_uart_eri_isr, /* SCI8 ERI (Receive error) */
            [14] = gpt_counter_overflow_isr, /* GPT5 COUNTER OVERFLOW (Overflow) */
            [15] = sci_uart_rxi_isr, /* SCI5 RXI (Receive data full) */
            [16] = sci_uart_txi_isr, /* SCI5 TXI (Transmit data empty) */
            [17] = sci_uart_tei_isr, /* SCI5 TEI (Transmit end) */
            [18] = sci_uart_eri_isr, /* SCI5 ERI (Receive error) */
            [19] = sci_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [20] = sci_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [21] = sci_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [22] = sci_uart_eri_isr, /* SCI9 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI7_RXI,GROUP0), /* SCI7 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI7_TXI,GROUP1), /* SCI7 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI7_TEI,GROUP2), /* SCI7 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI7_ERI,GROUP3), /* SCI7 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_IIC1_RXI,GROUP4), /* IIC1 RXI (Receive data full) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TXI,GROUP5), /* IIC1 TXI (Transmit data empty) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TEI,GROUP6), /* IIC1 TEI (Transmit end) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_IIC1_ERI,GROUP7), /* IIC1 ERI (Transfer error) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TXI,GROUP0), /* SCI4 TXI (Transmit data empty) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TEI,GROUP1), /* SCI4 TEI (Transmit end) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI8_RXI,GROUP2), /* SCI8 RXI (Receive data full) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TXI,GROUP3), /* SCI8 TXI (Transmit data empty) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI8_TEI,GROUP4), /* SCI8 TEI (Transmit end) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_SCI8_ERI,GROUP5), /* SCI8 ERI (Receive error) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_GPT5_COUNTER_OVERFLOW,GROUP6), /* GPT5 COUNTER OVERFLOW (Overflow) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_SCI5_RXI,GROUP7), /* SCI5 RXI (Receive data full) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TXI,GROUP0), /* SCI5 TXI (Transmit data empty) */
            [17] = BSP_PRV_VECT_ENUM(EVENT_SCI5_TEI,GROUP1), /* SCI5 TEI (Transmit end) */
            [18] = BSP_PRV_VECT_ENUM(EVENT_SCI5_ERI,GROUP2), /* SCI5 ERI (Receive error) */
            [19] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP3), /* SCI9 RXI (Receive data full) */
            [20] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP4), /* SCI9 TXI (Transmit data empty) */
            [21] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP5), /* SCI9 TEI (Transmit end) */
            [22] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP6), /* SCI9 ERI (Receive error) */
        };
        #endif
        #endif