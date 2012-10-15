#ifndef __ASM_R8A7779_H__
#define __ASM_R8A7779_H__

#include <linux/sh_clk.h>
#include <linux/pm_domain.h>

/* Pin Function Controller:
 * GPIO_FN_xx - GPIO used to select pin function
 * GPIO_GP_x_x - GPIO mapped to real I/O pin on CPU
 */
enum {
	GPIO_GP_0_0, GPIO_GP_0_1, GPIO_GP_0_2, GPIO_GP_0_3,
	GPIO_GP_0_4, GPIO_GP_0_5, GPIO_GP_0_6, GPIO_GP_0_7,
	GPIO_GP_0_8, GPIO_GP_0_9, GPIO_GP_0_10, GPIO_GP_0_11,
	GPIO_GP_0_12, GPIO_GP_0_13, GPIO_GP_0_14, GPIO_GP_0_15,
	GPIO_GP_0_16, GPIO_GP_0_17, GPIO_GP_0_18, GPIO_GP_0_19,
	GPIO_GP_0_20, GPIO_GP_0_21, GPIO_GP_0_22, GPIO_GP_0_23,
	GPIO_GP_0_24, GPIO_GP_0_25, GPIO_GP_0_26, GPIO_GP_0_27,
	GPIO_GP_0_28, GPIO_GP_0_29, GPIO_GP_0_30, GPIO_GP_0_31,

	GPIO_GP_1_0, GPIO_GP_1_1, GPIO_GP_1_2, GPIO_GP_1_3,
	GPIO_GP_1_4, GPIO_GP_1_5, GPIO_GP_1_6, GPIO_GP_1_7,
	GPIO_GP_1_8, GPIO_GP_1_9, GPIO_GP_1_10, GPIO_GP_1_11,
	GPIO_GP_1_12, GPIO_GP_1_13, GPIO_GP_1_14, GPIO_GP_1_15,
	GPIO_GP_1_16, GPIO_GP_1_17, GPIO_GP_1_18, GPIO_GP_1_19,
	GPIO_GP_1_20, GPIO_GP_1_21, GPIO_GP_1_22, GPIO_GP_1_23,
	GPIO_GP_1_24, GPIO_GP_1_25, GPIO_GP_1_26, GPIO_GP_1_27,
	GPIO_GP_1_28, GPIO_GP_1_29, GPIO_GP_1_30, GPIO_GP_1_31,

	GPIO_GP_2_0, GPIO_GP_2_1, GPIO_GP_2_2, GPIO_GP_2_3,
	GPIO_GP_2_4, GPIO_GP_2_5, GPIO_GP_2_6, GPIO_GP_2_7,
	GPIO_GP_2_8, GPIO_GP_2_9, GPIO_GP_2_10, GPIO_GP_2_11,
	GPIO_GP_2_12, GPIO_GP_2_13, GPIO_GP_2_14, GPIO_GP_2_15,
	GPIO_GP_2_16, GPIO_GP_2_17, GPIO_GP_2_18, GPIO_GP_2_19,
	GPIO_GP_2_20, GPIO_GP_2_21, GPIO_GP_2_22, GPIO_GP_2_23,
	GPIO_GP_2_24, GPIO_GP_2_25, GPIO_GP_2_26, GPIO_GP_2_27,
	GPIO_GP_2_28, GPIO_GP_2_29, GPIO_GP_2_30, GPIO_GP_2_31,

	GPIO_GP_3_0, GPIO_GP_3_1, GPIO_GP_3_2, GPIO_GP_3_3,
	GPIO_GP_3_4, GPIO_GP_3_5, GPIO_GP_3_6, GPIO_GP_3_7,
	GPIO_GP_3_8, GPIO_GP_3_9, GPIO_GP_3_10, GPIO_GP_3_11,
	GPIO_GP_3_12, GPIO_GP_3_13, GPIO_GP_3_14, GPIO_GP_3_15,
	GPIO_GP_3_16, GPIO_GP_3_17, GPIO_GP_3_18, GPIO_GP_3_19,
	GPIO_GP_3_20, GPIO_GP_3_21, GPIO_GP_3_22, GPIO_GP_3_23,
	GPIO_GP_3_24, GPIO_GP_3_25, GPIO_GP_3_26, GPIO_GP_3_27,
	GPIO_GP_3_28, GPIO_GP_3_29, GPIO_GP_3_30, GPIO_GP_3_31,

	GPIO_GP_4_0, GPIO_GP_4_1, GPIO_GP_4_2, GPIO_GP_4_3,
	GPIO_GP_4_4, GPIO_GP_4_5, GPIO_GP_4_6, GPIO_GP_4_7,
	GPIO_GP_4_8, GPIO_GP_4_9, GPIO_GP_4_10, GPIO_GP_4_11,
	GPIO_GP_4_12, GPIO_GP_4_13, GPIO_GP_4_14, GPIO_GP_4_15,
	GPIO_GP_4_16, GPIO_GP_4_17, GPIO_GP_4_18, GPIO_GP_4_19,
	GPIO_GP_4_20, GPIO_GP_4_21, GPIO_GP_4_22, GPIO_GP_4_23,
	GPIO_GP_4_24, GPIO_GP_4_25, GPIO_GP_4_26, GPIO_GP_4_27,
	GPIO_GP_4_28, GPIO_GP_4_29, GPIO_GP_4_30, GPIO_GP_4_31,

	GPIO_GP_5_0, GPIO_GP_5_1, GPIO_GP_5_2, GPIO_GP_5_3,
	GPIO_GP_5_4, GPIO_GP_5_5, GPIO_GP_5_6, GPIO_GP_5_7,
	GPIO_GP_5_8, GPIO_GP_5_9, GPIO_GP_5_10, GPIO_GP_5_11,
	GPIO_GP_5_12, GPIO_GP_5_13, GPIO_GP_5_14, GPIO_GP_5_15,
	GPIO_GP_5_16, GPIO_GP_5_17, GPIO_GP_5_18, GPIO_GP_5_19,
	GPIO_GP_5_20, GPIO_GP_5_21, GPIO_GP_5_22, GPIO_GP_5_23,
	GPIO_GP_5_24, GPIO_GP_5_25, GPIO_GP_5_26, GPIO_GP_5_27,
	GPIO_GP_5_28, GPIO_GP_5_29, GPIO_GP_5_30, GPIO_GP_5_31,

	GPIO_GP_6_0, GPIO_GP_6_1, GPIO_GP_6_2, GPIO_GP_6_3,
	GPIO_GP_6_4, GPIO_GP_6_5, GPIO_GP_6_6, GPIO_GP_6_7,
	GPIO_GP_6_8,

	GPIO_FN_AVS1, GPIO_FN_AVS2, GPIO_FN_A17, GPIO_FN_A18,
	GPIO_FN_A19,

	/* IPSR0 */
	GPIO_FN_PENC2, GPIO_FN_SCK0, GPIO_FN_PWM1, GPIO_FN_PWMFSW0,
	GPIO_FN_SCIF_CLK, GPIO_FN_TCLK0_C, GPIO_FN_BS, GPIO_FN_SD1_DAT2,
	GPIO_FN_MMC0_D2, GPIO_FN_FD2, GPIO_FN_ATADIR0, GPIO_FN_SDSELF,
	GPIO_FN_HCTS1, GPIO_FN_TX4_C, GPIO_FN_A0, GPIO_FN_SD1_DAT3,
	GPIO_FN_MMC0_D3, GPIO_FN_FD3, GPIO_FN_A20, GPIO_FN_TX5_D,
	GPIO_FN_HSPI_TX2_B, GPIO_FN_A21, GPIO_FN_SCK5_D, GPIO_FN_HSPI_CLK2_B,
	GPIO_FN_A22, GPIO_FN_RX5_D, GPIO_FN_HSPI_RX2_B, GPIO_FN_VI1_R0,
	GPIO_FN_A23, GPIO_FN_FCLE, GPIO_FN_HSPI_CLK2, GPIO_FN_VI1_R1,
	GPIO_FN_A24, GPIO_FN_SD1_CD, GPIO_FN_MMC0_D4, GPIO_FN_FD4,
	GPIO_FN_HSPI_CS2, GPIO_FN_VI1_R2, GPIO_FN_SSI_WS78_B, GPIO_FN_A25,
	GPIO_FN_SD1_WP, GPIO_FN_MMC0_D5, GPIO_FN_FD5, GPIO_FN_HSPI_RX2,
	GPIO_FN_VI1_R3, GPIO_FN_TX5_B, GPIO_FN_SSI_SDATA7_B, GPIO_FN_CTS0_B,
	GPIO_FN_CLKOUT, GPIO_FN_TX3C_IRDA_TX_C, GPIO_FN_PWM0_B, GPIO_FN_CS0,
	GPIO_FN_HSPI_CS2_B, GPIO_FN_CS1_A26, GPIO_FN_HSPI_TX2,
	GPIO_FN_SDSELF_B, GPIO_FN_RD_WR, GPIO_FN_FWE, GPIO_FN_ATAG0,
	GPIO_FN_VI1_R7, GPIO_FN_HRTS1, GPIO_FN_RX4_C,

	/* IPSR1 */
	GPIO_FN_EX_CS0, GPIO_FN_RX3_C_IRDA_RX_C, GPIO_FN_MMC0_D6,
	GPIO_FN_FD6, GPIO_FN_EX_CS1, GPIO_FN_MMC0_D7, GPIO_FN_FD7,
	GPIO_FN_EX_CS2, GPIO_FN_SD1_CLK, GPIO_FN_MMC0_CLK, GPIO_FN_FALE,
	GPIO_FN_ATACS00, GPIO_FN_EX_CS3, GPIO_FN_SD1_CMD, GPIO_FN_MMC0_CMD,
	GPIO_FN_FRE, GPIO_FN_ATACS10, GPIO_FN_VI1_R4, GPIO_FN_RX5_B,
	GPIO_FN_HSCK1, GPIO_FN_SSI_SDATA8_B, GPIO_FN_RTS0_B_TANS_B,
	GPIO_FN_SSI_SDATA9, GPIO_FN_EX_CS4, GPIO_FN_SD1_DAT0, GPIO_FN_MMC0_D0,
	GPIO_FN_FD0, GPIO_FN_ATARD0, GPIO_FN_VI1_R5, GPIO_FN_SCK5_B,
	GPIO_FN_HTX1, GPIO_FN_TX2_E, GPIO_FN_TX0_B, GPIO_FN_SSI_SCK9,
	GPIO_FN_EX_CS5, GPIO_FN_SD1_DAT1, GPIO_FN_MMC0_D1, GPIO_FN_FD1,
	GPIO_FN_ATAWR0, GPIO_FN_VI1_R6, GPIO_FN_HRX1, GPIO_FN_RX2_E,
	GPIO_FN_RX0_B, GPIO_FN_SSI_WS9, GPIO_FN_MLB_CLK, GPIO_FN_PWM2,
	GPIO_FN_SCK4, GPIO_FN_MLB_SIG, GPIO_FN_PWM3, GPIO_FN_TX4,
	GPIO_FN_MLB_DAT, GPIO_FN_PWM4, GPIO_FN_RX4, GPIO_FN_HTX0,
	GPIO_FN_TX1, GPIO_FN_SDATA, GPIO_FN_CTS0_C, GPIO_FN_SUB_TCK,
	GPIO_FN_CC5_STATE2, GPIO_FN_CC5_STATE10, GPIO_FN_CC5_STATE18,
	GPIO_FN_CC5_STATE26, GPIO_FN_CC5_STATE34,

	/* IPSR2 */
	GPIO_FN_HRX0, GPIO_FN_RX1, GPIO_FN_SCKZ, GPIO_FN_RTS0_C_TANS_C,
	GPIO_FN_SUB_TDI, GPIO_FN_CC5_STATE3, GPIO_FN_CC5_STATE11,
	GPIO_FN_CC5_STATE19, GPIO_FN_CC5_STATE27, GPIO_FN_CC5_STATE35,
	GPIO_FN_HSCK0, GPIO_FN_SCK1, GPIO_FN_MTS, GPIO_FN_PWM5,
	GPIO_FN_SCK0_C, GPIO_FN_SSI_SDATA9_B, GPIO_FN_SUB_TDO,
	GPIO_FN_CC5_STATE0, GPIO_FN_CC5_STATE8, GPIO_FN_CC5_STATE16,
	GPIO_FN_CC5_STATE24, GPIO_FN_CC5_STATE32, GPIO_FN_HCTS0, GPIO_FN_CTS1,
	GPIO_FN_STM, GPIO_FN_PWM0_D, GPIO_FN_RX0_C, GPIO_FN_SCIF_CLK_C,
	GPIO_FN_SUB_TRST, GPIO_FN_TCLK1_B, GPIO_FN_CC5_OSCOUT, GPIO_FN_HRTS0,
	GPIO_FN_RTS1_TANS, GPIO_FN_MDATA, GPIO_FN_TX0_C, GPIO_FN_SUB_TMS,
	GPIO_FN_CC5_STATE1, GPIO_FN_CC5_STATE9, GPIO_FN_CC5_STATE17,
	GPIO_FN_CC5_STATE25, GPIO_FN_CC5_STATE33, GPIO_FN_DU0_DR0,
	GPIO_FN_LCDOUT0, GPIO_FN_DREQ0, GPIO_FN_GPS_CLK_B, GPIO_FN_AUDATA0,
	GPIO_FN_TX5_C, GPIO_FN_DU0_DR1,	GPIO_FN_LCDOUT1, GPIO_FN_DACK0,
	GPIO_FN_DRACK0, GPIO_FN_GPS_SIGN_B, GPIO_FN_AUDATA1, GPIO_FN_RX5_C,
	GPIO_FN_DU0_DR2, GPIO_FN_LCDOUT2, GPIO_FN_DU0_DR3, GPIO_FN_LCDOUT3,
	GPIO_FN_DU0_DR4, GPIO_FN_LCDOUT4, GPIO_FN_DU0_DR5, GPIO_FN_LCDOUT5,
	GPIO_FN_DU0_DR6, GPIO_FN_LCDOUT6, GPIO_FN_DU0_DR7, GPIO_FN_LCDOUT7,
	GPIO_FN_DU0_DG0, GPIO_FN_LCDOUT8, GPIO_FN_DREQ1, GPIO_FN_SCL2,
	GPIO_FN_AUDATA2,

	/* IPSR3 */
	GPIO_FN_DU0_DG1, GPIO_FN_LCDOUT9, GPIO_FN_DACK1, GPIO_FN_SDA2,
	GPIO_FN_AUDATA3, GPIO_FN_DU0_DG2, GPIO_FN_LCDOUT10, GPIO_FN_DU0_DG3,
	GPIO_FN_LCDOUT11, GPIO_FN_DU0_DG4, GPIO_FN_LCDOUT12, GPIO_FN_DU0_DG5,
	GPIO_FN_LCDOUT13, GPIO_FN_DU0_DG6, GPIO_FN_LCDOUT14, GPIO_FN_DU0_DG7,
	GPIO_FN_LCDOUT15, GPIO_FN_DU0_DB0, GPIO_FN_LCDOUT16, GPIO_FN_EX_WAIT1,
	GPIO_FN_SCL1, GPIO_FN_TCLK1, GPIO_FN_AUDATA4, GPIO_FN_DU0_DB1,
	GPIO_FN_LCDOUT17, GPIO_FN_EX_WAIT2, GPIO_FN_SDA1, GPIO_FN_GPS_MAG_B,
	GPIO_FN_AUDATA5, GPIO_FN_SCK5_C, GPIO_FN_DU0_DB2, GPIO_FN_LCDOUT18,
	GPIO_FN_DU0_DB3, GPIO_FN_LCDOUT19, GPIO_FN_DU0_DB4, GPIO_FN_LCDOUT20,
	GPIO_FN_DU0_DB5, GPIO_FN_LCDOUT21, GPIO_FN_DU0_DB6, GPIO_FN_LCDOUT22,
	GPIO_FN_DU0_DB7, GPIO_FN_LCDOUT23, GPIO_FN_DU0_DOTCLKIN,
	GPIO_FN_QSTVA_QVS, GPIO_FN_TX3_D_IRDA_TX_D, GPIO_FN_SCL3_B,
	GPIO_FN_DU0_DOTCLKOUT0, GPIO_FN_QCLK, GPIO_FN_DU0_DOTCLKOUT1,
	GPIO_FN_QSTVB_QVE, GPIO_FN_RX3_D_IRDA_RX_D, GPIO_FN_SDA3_B,
	GPIO_FN_SDA2_C, GPIO_FN_DACK0_B, GPIO_FN_DRACK0_B,
	GPIO_FN_DU0_EXHSYNC_DU0_HSYNC, GPIO_FN_QSTH_QHS,
	GPIO_FN_DU0_EXVSYNC_DU0_VSYNC, GPIO_FN_QSTB_QHE,
	GPIO_FN_DU0_EXODDF_DU0_ODDF_DISP_CDE, GPIO_FN_QCPV_QDE,
	GPIO_FN_CAN1_TX, GPIO_FN_TX2_C, GPIO_FN_SCL2_C, GPIO_FN_REMOCON,

	/* IPSR4 */
	GPIO_FN_DU0_DISP, GPIO_FN_QPOLA, GPIO_FN_CAN_CLK_C, GPIO_FN_SCK2_C,
	GPIO_FN_DU0_CDE, GPIO_FN_QPOLB, GPIO_FN_CAN1_RX, GPIO_FN_RX2_C,
	GPIO_FN_DREQ0_B, GPIO_FN_SSI_SCK78_B, GPIO_FN_SCK0_B, GPIO_FN_DU1_DR0,
	GPIO_FN_VI2_DATA0_VI2_B0, GPIO_FN_PWM6, GPIO_FN_SD3_CLK,
	GPIO_FN_TX3_E_IRDA_TX_E, GPIO_FN_AUDCK, GPIO_FN_PWMFSW0_B,
	GPIO_FN_DU1_DR1, GPIO_FN_VI2_DATA1_VI2_B1, GPIO_FN_PWM0,
	GPIO_FN_SD3_CMD, GPIO_FN_RX3_E_IRDA_RX_E, GPIO_FN_AUDSYNC,
	GPIO_FN_CTS0_D, GPIO_FN_DU1_DR2, GPIO_FN_VI2_G0, GPIO_FN_DU1_DR3,
	GPIO_FN_VI2_G1, GPIO_FN_DU1_DR4, GPIO_FN_VI2_G2, GPIO_FN_DU1_DR5,
	GPIO_FN_VI2_G3, GPIO_FN_DU1_DR6, GPIO_FN_VI2_G4, GPIO_FN_DU1_DR7,
	GPIO_FN_VI2_G5, GPIO_FN_DU1_DG0, GPIO_FN_VI2_DATA2_VI2_B2,
	GPIO_FN_SCL1_B, GPIO_FN_SD3_DAT2, GPIO_FN_SCK3_E, GPIO_FN_AUDATA6,
	GPIO_FN_TX0_D, GPIO_FN_DU1_DG1, GPIO_FN_VI2_DATA3_VI2_B3,
	GPIO_FN_SDA1_B, GPIO_FN_SD3_DAT3, GPIO_FN_SCK5, GPIO_FN_AUDATA7,
	GPIO_FN_RX0_D, GPIO_FN_DU1_DG2,	GPIO_FN_VI2_G6, GPIO_FN_DU1_DG3,
	GPIO_FN_VI2_G7, GPIO_FN_DU1_DG4, GPIO_FN_VI2_R0, GPIO_FN_DU1_DG5,
	GPIO_FN_VI2_R1, GPIO_FN_DU1_DG6, GPIO_FN_VI2_R2, GPIO_FN_DU1_DG7,
	GPIO_FN_VI2_R3, GPIO_FN_DU1_DB0, GPIO_FN_VI2_DATA4_VI2_B4,
	GPIO_FN_SCL2_B, GPIO_FN_SD3_DAT0, GPIO_FN_TX5, GPIO_FN_SCK0_D,

	/* IPSR5 */
	GPIO_FN_DU1_DB1, GPIO_FN_VI2_DATA5_VI2_B5, GPIO_FN_SDA2_B,
	GPIO_FN_SD3_DAT1, GPIO_FN_RX5, GPIO_FN_RTS0_D_TANS_D,
	GPIO_FN_DU1_DB2, GPIO_FN_VI2_R4, GPIO_FN_DU1_DB3, GPIO_FN_VI2_R5,
	GPIO_FN_DU1_DB4, GPIO_FN_VI2_R6, GPIO_FN_DU1_DB5, GPIO_FN_VI2_R7,
	GPIO_FN_DU1_DB6, GPIO_FN_SCL2_D, GPIO_FN_DU1_DB7, GPIO_FN_SDA2_D,
	GPIO_FN_DU1_DOTCLKIN, GPIO_FN_VI2_CLKENB, GPIO_FN_HSPI_CS1,
	GPIO_FN_SCL1_D, GPIO_FN_DU1_DOTCLKOUT, GPIO_FN_VI2_FIELD,
	GPIO_FN_SDA1_D, GPIO_FN_DU1_EXHSYNC_DU1_HSYNC, GPIO_FN_VI2_HSYNC,
	GPIO_FN_VI3_HSYNC, GPIO_FN_DU1_EXVSYNC_DU1_VSYNC, GPIO_FN_VI2_VSYNC,
	GPIO_FN_VI3_VSYNC, GPIO_FN_DU1_EXODDF_DU1_ODDF_DISP_CDE,
	GPIO_FN_VI2_CLK, GPIO_FN_TX3_B_IRDA_TX_B, GPIO_FN_SD3_CD,
	GPIO_FN_HSPI_TX1, GPIO_FN_VI1_CLKENB, GPIO_FN_VI3_CLKENB,
	GPIO_FN_AUDIO_CLKC, GPIO_FN_TX2_D, GPIO_FN_SPEEDIN,
	GPIO_FN_GPS_SIGN_D, GPIO_FN_DU1_DISP, GPIO_FN_VI2_DATA6_VI2_B6,
	GPIO_FN_TCLK0, GPIO_FN_QSTVA_B_QVS_B, GPIO_FN_HSPI_CLK1,
	GPIO_FN_SCK2_D, GPIO_FN_AUDIO_CLKOUT_B, GPIO_FN_GPS_MAG_D,
	GPIO_FN_DU1_CDE, GPIO_FN_VI2_DATA7_VI2_B7, GPIO_FN_RX3_B_IRDA_RX_B,
	GPIO_FN_SD3_WP, GPIO_FN_HSPI_RX1, GPIO_FN_VI1_FIELD, GPIO_FN_VI3_FIELD,
	GPIO_FN_AUDIO_CLKOUT, GPIO_FN_RX2_D, GPIO_FN_GPS_CLK_C,
	GPIO_FN_GPS_CLK_D, GPIO_FN_AUDIO_CLKA, GPIO_FN_CAN_TXCLK,
	GPIO_FN_AUDIO_CLKB, GPIO_FN_USB_OVC2, GPIO_FN_CAN_DEBUGOUT0,
	GPIO_FN_MOUT0,

	/* IPSR6 */
	GPIO_FN_SSI_SCK0129, GPIO_FN_CAN_DEBUGOUT1, GPIO_FN_MOUT1,
	GPIO_FN_SSI_WS0129, GPIO_FN_CAN_DEBUGOUT2, GPIO_FN_MOUT2,
	GPIO_FN_SSI_SDATA0, GPIO_FN_CAN_DEBUGOUT3, GPIO_FN_MOUT5,
	GPIO_FN_SSI_SDATA1, GPIO_FN_CAN_DEBUGOUT4, GPIO_FN_MOUT6,
	GPIO_FN_SSI_SDATA2, GPIO_FN_CAN_DEBUGOUT5, GPIO_FN_SSI_SCK34,
	GPIO_FN_CAN_DEBUGOUT6, GPIO_FN_CAN0_TX_B, GPIO_FN_IERX,
	GPIO_FN_SSI_SCK9_C, GPIO_FN_SSI_WS34, GPIO_FN_CAN_DEBUGOUT7,
	GPIO_FN_CAN0_RX_B, GPIO_FN_IETX, GPIO_FN_SSI_WS9_C,
	GPIO_FN_SSI_SDATA3, GPIO_FN_PWM0_C, GPIO_FN_CAN_DEBUGOUT8,
	GPIO_FN_CAN_CLK_B, GPIO_FN_IECLK, GPIO_FN_SCIF_CLK_B, GPIO_FN_TCLK0_B,
	GPIO_FN_SSI_SDATA4, GPIO_FN_CAN_DEBUGOUT9, GPIO_FN_SSI_SDATA9_C,
	GPIO_FN_SSI_SCK5, GPIO_FN_ADICLK, GPIO_FN_CAN_DEBUGOUT10,
	GPIO_FN_SCK3, GPIO_FN_TCLK0_D, GPIO_FN_SSI_WS5, GPIO_FN_ADICS_SAMP,
	GPIO_FN_CAN_DEBUGOUT11, GPIO_FN_TX3_IRDA_TX, GPIO_FN_SSI_SDATA5,
	GPIO_FN_ADIDATA, GPIO_FN_CAN_DEBUGOUT12, GPIO_FN_RX3_IRDA_RX,
	GPIO_FN_SSI_SCK6, GPIO_FN_ADICHS0, GPIO_FN_CAN0_TX, GPIO_FN_IERX_B,

	/* IPSR7 */
	GPIO_FN_SSI_WS6, GPIO_FN_ADICHS1, GPIO_FN_CAN0_RX, GPIO_FN_IETX_B,
	GPIO_FN_SSI_SDATA6, GPIO_FN_ADICHS2, GPIO_FN_CAN_CLK, GPIO_FN_IECLK_B,
	GPIO_FN_SSI_SCK78, GPIO_FN_CAN_DEBUGOUT13, GPIO_FN_IRQ0_B,
	GPIO_FN_SSI_SCK9_B, GPIO_FN_HSPI_CLK1_C, GPIO_FN_SSI_WS78,
	GPIO_FN_CAN_DEBUGOUT14, GPIO_FN_IRQ1_B, GPIO_FN_SSI_WS9_B,
	GPIO_FN_HSPI_CS1_C, GPIO_FN_SSI_SDATA7, GPIO_FN_CAN_DEBUGOUT15,
	GPIO_FN_IRQ2_B, GPIO_FN_TCLK1_C, GPIO_FN_HSPI_TX1_C,
	GPIO_FN_SSI_SDATA8, GPIO_FN_VSP, GPIO_FN_IRQ3_B, GPIO_FN_HSPI_RX1_C,
	GPIO_FN_SD0_CLK, GPIO_FN_ATACS01, GPIO_FN_SCK1_B, GPIO_FN_SD0_CMD,
	GPIO_FN_ATACS11, GPIO_FN_TX1_B, GPIO_FN_CC5_TDO, GPIO_FN_SD0_DAT0,
	GPIO_FN_ATADIR1, GPIO_FN_RX1_B, GPIO_FN_CC5_TRST, GPIO_FN_SD0_DAT1,
	GPIO_FN_ATAG1, GPIO_FN_SCK2_B, GPIO_FN_CC5_TMS, GPIO_FN_SD0_DAT2,
	GPIO_FN_ATARD1,	GPIO_FN_TX2_B, GPIO_FN_CC5_TCK, GPIO_FN_SD0_DAT3,
	GPIO_FN_ATAWR1,	GPIO_FN_RX2_B, GPIO_FN_CC5_TDI, GPIO_FN_SD0_CD,
	GPIO_FN_DREQ2,	GPIO_FN_RTS1_B_TANS_B, GPIO_FN_SD0_WP, GPIO_FN_DACK2,
	GPIO_FN_CTS1_B,

	/* IPSR8 */
	GPIO_FN_HSPI_CLK0, GPIO_FN_CTS0, GPIO_FN_USB_OVC0, GPIO_FN_AD_CLK,
	GPIO_FN_CC5_STATE4, GPIO_FN_CC5_STATE12, GPIO_FN_CC5_STATE20,
	GPIO_FN_CC5_STATE28, GPIO_FN_CC5_STATE36, GPIO_FN_HSPI_CS0,
	GPIO_FN_RTS0_TANS, GPIO_FN_USB_OVC1, GPIO_FN_AD_DI,
	GPIO_FN_CC5_STATE5, GPIO_FN_CC5_STATE13, GPIO_FN_CC5_STATE21,
	GPIO_FN_CC5_STATE29, GPIO_FN_CC5_STATE37, GPIO_FN_HSPI_TX0,
	GPIO_FN_TX0, GPIO_FN_CAN_DEBUG_HW_TRIGGER, GPIO_FN_AD_DO,
	GPIO_FN_CC5_STATE6, GPIO_FN_CC5_STATE14, GPIO_FN_CC5_STATE22,
	GPIO_FN_CC5_STATE30, GPIO_FN_CC5_STATE38, GPIO_FN_HSPI_RX0,
	GPIO_FN_RX0, GPIO_FN_CAN_STEP0, GPIO_FN_AD_NCS, GPIO_FN_CC5_STATE7,
	GPIO_FN_CC5_STATE15, GPIO_FN_CC5_STATE23, GPIO_FN_CC5_STATE31,
	GPIO_FN_CC5_STATE39, GPIO_FN_FMCLK, GPIO_FN_RDS_CLK, GPIO_FN_PCMOE,
	GPIO_FN_BPFCLK, GPIO_FN_PCMWE, GPIO_FN_FMIN, GPIO_FN_RDS_DATA,
	GPIO_FN_VI0_CLK, GPIO_FN_MMC1_CLK, GPIO_FN_VI0_CLKENB, GPIO_FN_TX1_C,
	GPIO_FN_HTX1_B, GPIO_FN_MT1_SYNC, GPIO_FN_VI0_FIELD, GPIO_FN_RX1_C,
	GPIO_FN_HRX1_B, GPIO_FN_VI0_HSYNC, GPIO_FN_VI0_DATA0_B_VI0_B0_B,
	GPIO_FN_CTS1_C, GPIO_FN_TX4_D, GPIO_FN_MMC1_CMD, GPIO_FN_HSCK1_B,
	GPIO_FN_VI0_VSYNC, GPIO_FN_VI0_DATA1_B_VI0_B1_B,
	GPIO_FN_RTS1_C_TANS_C, GPIO_FN_RX4_D, GPIO_FN_PWMFSW0_C,

	/* IPSR9 */
	GPIO_FN_VI0_DATA0_VI0_B0, GPIO_FN_HRTS1_B, GPIO_FN_MT1_VCXO,
	GPIO_FN_VI0_DATA1_VI0_B1, GPIO_FN_HCTS1_B, GPIO_FN_MT1_PWM,
	GPIO_FN_VI0_DATA2_VI0_B2, GPIO_FN_MMC1_D0, GPIO_FN_VI0_DATA3_VI0_B3,
	GPIO_FN_MMC1_D1, GPIO_FN_VI0_DATA4_VI0_B4, GPIO_FN_MMC1_D2,
	GPIO_FN_VI0_DATA5_VI0_B5, GPIO_FN_MMC1_D3, GPIO_FN_VI0_DATA6_VI0_B6,
	GPIO_FN_MMC1_D4, GPIO_FN_ARM_TRACEDATA_0, GPIO_FN_VI0_DATA7_VI0_B7,
	GPIO_FN_MMC1_D5, GPIO_FN_ARM_TRACEDATA_1, GPIO_FN_VI0_G0,
	GPIO_FN_SSI_SCK78_C, GPIO_FN_IRQ0, GPIO_FN_ARM_TRACEDATA_2,
	GPIO_FN_VI0_G1, GPIO_FN_SSI_WS78_C, GPIO_FN_IRQ1,
	GPIO_FN_ARM_TRACEDATA_3, GPIO_FN_VI0_G2, GPIO_FN_ETH_TXD1,
	GPIO_FN_MMC1_D6, GPIO_FN_ARM_TRACEDATA_4, GPIO_FN_TS_SPSYNC0,
	GPIO_FN_VI0_G3, GPIO_FN_ETH_CRS_DV, GPIO_FN_MMC1_D7,
	GPIO_FN_ARM_TRACEDATA_5, GPIO_FN_TS_SDAT0, GPIO_FN_VI0_G4,
	GPIO_FN_ETH_TX_EN, GPIO_FN_SD2_DAT0_B, GPIO_FN_ARM_TRACEDATA_6,
	GPIO_FN_VI0_G5,	GPIO_FN_ETH_RX_ER, GPIO_FN_SD2_DAT1_B,
	GPIO_FN_ARM_TRACEDATA_7, GPIO_FN_VI0_G6, GPIO_FN_ETH_RXD0,
	GPIO_FN_SD2_DAT2_B, GPIO_FN_ARM_TRACEDATA_8, GPIO_FN_VI0_G7,
	GPIO_FN_ETH_RXD1, GPIO_FN_SD2_DAT3_B, GPIO_FN_ARM_TRACEDATA_9,

	/* IPSR10 */
	GPIO_FN_VI0_R0, GPIO_FN_SSI_SDATA7_C, GPIO_FN_SCK1_C, GPIO_FN_DREQ1_B,
	GPIO_FN_ARM_TRACEDATA_10, GPIO_FN_DREQ0_C, GPIO_FN_VI0_R1,
	GPIO_FN_SSI_SDATA8_C, GPIO_FN_DACK1_B, GPIO_FN_ARM_TRACEDATA_11,
	GPIO_FN_DACK0_C, GPIO_FN_DRACK0_C, GPIO_FN_VI0_R2, GPIO_FN_ETH_LINK,
	GPIO_FN_SD2_CLK_B, GPIO_FN_IRQ2, GPIO_FN_ARM_TRACEDATA_12,
	GPIO_FN_VI0_R3, GPIO_FN_ETH_MAGIC, GPIO_FN_SD2_CMD_B, GPIO_FN_IRQ3,
	GPIO_FN_ARM_TRACEDATA_13, GPIO_FN_VI0_R4, GPIO_FN_ETH_REFCLK,
	GPIO_FN_SD2_CD_B, GPIO_FN_HSPI_CLK1_B, GPIO_FN_ARM_TRACEDATA_14,
	GPIO_FN_MT1_CLK, GPIO_FN_TS_SCK0, GPIO_FN_VI0_R5, GPIO_FN_ETH_TXD0,
	GPIO_FN_SD2_WP_B, GPIO_FN_HSPI_CS1_B, GPIO_FN_ARM_TRACEDATA_15,
	GPIO_FN_MT1_D, GPIO_FN_TS_SDEN0, GPIO_FN_VI0_R6, GPIO_FN_ETH_MDC,
	GPIO_FN_DREQ2_C, GPIO_FN_HSPI_TX1_B, GPIO_FN_TRACECLK,
	GPIO_FN_MT1_BEN, GPIO_FN_PWMFSW0_D, GPIO_FN_VI0_R7, GPIO_FN_ETH_MDIO,
	GPIO_FN_DACK2_C, GPIO_FN_HSPI_RX1_B, GPIO_FN_SCIF_CLK_D,
	GPIO_FN_TRACECTL, GPIO_FN_MT1_PEN, GPIO_FN_VI1_CLK, GPIO_FN_SIM_D,
	GPIO_FN_SDA3, GPIO_FN_VI1_HSYNC, GPIO_FN_VI3_CLK, GPIO_FN_SSI_SCK4,
	GPIO_FN_GPS_SIGN_C, GPIO_FN_PWMFSW0_E, GPIO_FN_VI1_VSYNC,
	GPIO_FN_AUDIO_CLKOUT_C, GPIO_FN_SSI_WS4, GPIO_FN_SIM_CLK,
	GPIO_FN_GPS_MAG_C, GPIO_FN_SPV_TRST, GPIO_FN_SCL3,

	/* IPSR11 */
	GPIO_FN_VI1_DATA0_VI1_B0, GPIO_FN_SD2_DAT0, GPIO_FN_SIM_RST,
	GPIO_FN_SPV_TCK, GPIO_FN_ADICLK_B, GPIO_FN_VI1_DATA1_VI1_B1,
	GPIO_FN_SD2_DAT1, GPIO_FN_MT0_CLK, GPIO_FN_SPV_TMS,
	GPIO_FN_ADICS_B_SAMP_B, GPIO_FN_VI1_DATA2_VI1_B2, GPIO_FN_SD2_DAT2,
	GPIO_FN_MT0_D, GPIO_FN_SPVTDI, GPIO_FN_ADIDATA_B,
	GPIO_FN_VI1_DATA3_VI1_B3, GPIO_FN_SD2_DAT3, GPIO_FN_MT0_BEN,
	GPIO_FN_SPV_TDO, GPIO_FN_ADICHS0_B, GPIO_FN_VI1_DATA4_VI1_B4,
	GPIO_FN_SD2_CLK, GPIO_FN_MT0_PEN, GPIO_FN_SPA_TRST,
	GPIO_FN_HSPI_CLK1_D, GPIO_FN_ADICHS1_B, GPIO_FN_VI1_DATA5_VI1_B5,
	GPIO_FN_SD2_CMD, GPIO_FN_MT0_SYNC, GPIO_FN_SPA_TCK,
	GPIO_FN_HSPI_CS1_D, GPIO_FN_ADICHS2_B, GPIO_FN_VI1_DATA6_VI1_B6,
	GPIO_FN_SD2_CD, GPIO_FN_MT0_VCXO, GPIO_FN_SPA_TMS, GPIO_FN_HSPI_TX1_D,
	GPIO_FN_VI1_DATA7_VI1_B7, GPIO_FN_SD2_WP, GPIO_FN_MT0_PWM,
	GPIO_FN_SPA_TDI, GPIO_FN_HSPI_RX1_D, GPIO_FN_VI1_G0, GPIO_FN_VI3_DATA0,
	GPIO_FN_DU1_DOTCLKOUT1, GPIO_FN_TS_SCK1, GPIO_FN_DREQ2_B, GPIO_FN_TX2,
	GPIO_FN_SPA_TDO, GPIO_FN_HCTS0_B, GPIO_FN_VI1_G1, GPIO_FN_VI3_DATA1,
	GPIO_FN_SSI_SCK1, GPIO_FN_TS_SDEN1, GPIO_FN_DACK2_B, GPIO_FN_RX2,
	GPIO_FN_HRTS0_B,

	/* IPSR12 */
	GPIO_FN_VI1_G2, GPIO_FN_VI3_DATA2, GPIO_FN_SSI_WS1, GPIO_FN_TS_SPSYNC1,
	GPIO_FN_SCK2, GPIO_FN_HSCK0_B, GPIO_FN_VI1_G3, GPIO_FN_VI3_DATA3,
	GPIO_FN_SSI_SCK2, GPIO_FN_TS_SDAT1, GPIO_FN_SCL1_C, GPIO_FN_HTX0_B,
	GPIO_FN_VI1_G4, GPIO_FN_VI3_DATA4, GPIO_FN_SSI_WS2, GPIO_FN_SDA1_C,
	GPIO_FN_SIM_RST_B, GPIO_FN_HRX0_B, GPIO_FN_VI1_G5, GPIO_FN_VI3_DATA5,
	GPIO_FN_GPS_CLK, GPIO_FN_FSE, GPIO_FN_TX4_B, GPIO_FN_SIM_D_B,
	GPIO_FN_VI1_G6, GPIO_FN_VI3_DATA6, GPIO_FN_GPS_SIGN, GPIO_FN_FRB,
	GPIO_FN_RX4_B, GPIO_FN_SIM_CLK_B, GPIO_FN_VI1_G7, GPIO_FN_VI3_DATA7,
	GPIO_FN_GPS_MAG, GPIO_FN_FCE, GPIO_FN_SCK4_B,
};

struct platform_device;

struct r8a7779_pm_ch {
	unsigned long chan_offs;
	unsigned int chan_bit;
	unsigned int isr_bit;
};

struct r8a7779_pm_domain {
	struct generic_pm_domain genpd;
	struct r8a7779_pm_ch ch;
};

static inline struct r8a7779_pm_ch *to_r8a7779_ch(struct generic_pm_domain *d)
{
	return &container_of(d, struct r8a7779_pm_domain, genpd)->ch;
}

extern int r8a7779_sysc_power_down(struct r8a7779_pm_ch *r8a7779_ch);
extern int r8a7779_sysc_power_up(struct r8a7779_pm_ch *r8a7779_ch);

#ifdef CONFIG_PM
extern struct r8a7779_pm_domain r8a7779_sh4a;
extern struct r8a7779_pm_domain r8a7779_sgx;
extern struct r8a7779_pm_domain r8a7779_vdp1;
extern struct r8a7779_pm_domain r8a7779_impx3;

extern void r8a7779_init_pm_domain(struct r8a7779_pm_domain *r8a7779_pd);
extern void r8a7779_add_device_to_domain(struct r8a7779_pm_domain *r8a7779_pd,
					struct platform_device *pdev);
#else
#define r8a7779_init_pm_domain(pd) do { } while (0)
#define r8a7779_add_device_to_domain(pd, pdev) do { } while (0)
#endif /* CONFIG_PM */

extern struct smp_operations r8a7779_smp_ops;

#endif /* __ASM_R8A7779_H__ */
