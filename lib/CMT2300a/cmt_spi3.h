#ifndef __CMT_SPI3_H
#define __CMT_SPI3_H

//#include <linux/delay.h>
//#include <ty_typedefs.h>
#include "Arduino.h"

/*
extern void ty_set_gpio1_in(void);
extern void ty_set_gpio2_in(void);
extern void ty_set_gpio3_in(void);
extern int ty_read_gpio1_pin(void);
extern int ty_read_gpio2_pin(void);
extern int ty_read_gpio3_pin(void);
extern void ty_set_csb_out(void);
extern void ty_set_fcsb_out(void);
extern void ty_set_sclk_out(void);
extern void ty_set_sdio_out(void);
extern void ty_set_sdio_in(void);
extern void ty_set_csb_h(void);
extern void ty_set_csb_l(void);
extern void ty_set_fcsb_h(void);
extern void ty_set_fcsb_l(void);
extern void ty_set_sclk_h(void);
extern void ty_set_sclk_l(void);
extern void ty_set_sdio_h(void);
extern void ty_set_sdio_l(void);
extern int ty_read_sdio_pin(void);
*/
/* ************************************************************************
*  The following need to be modified by user
*  ************************************************************************ */
#define t_nSysTickCount 	millis() //ͬ��ʱ�ӣ��û����Բ��ö���

#define CMT2300A_GPIO2_PIN						PIN_P22
#define CMT2300A_GPIO1_PIN						PIN_P20 //34  //interupt pin

#define CMT2300A_SPI_SCLK_PIN					PIN_P14
#define CMT2300A_SPI_MOSI_PIN					PIN_P16

#define CMT2300A_SPI_CSB_PIN					PIN_P6
#define CMT2300A_SPI_FCSB_PIN					PIN_P26

#define cmt_spi3_csb_out()   pinMode(CMT2300A_SPI_CSB_PIN, OUTPUT)
#define cmt_spi3_csb_1()	 digitalWrite(CMT2300A_SPI_CSB_PIN,HIGH)
#define cmt_spi3_csb_0()	 digitalWrite(CMT2300A_SPI_CSB_PIN,LOW)
#define cmt_spi3_fcsb_out()  pinMode(CMT2300A_SPI_FCSB_PIN, OUTPUT)
#define cmt_spi3_fcsb_1()	 digitalWrite(CMT2300A_SPI_FCSB_PIN,HIGH)
#define cmt_spi3_fcsb_0()	 digitalWrite(CMT2300A_SPI_FCSB_PIN,LOW)
#define cmt_spi3_sclk_out()  pinMode(CMT2300A_SPI_SCLK_PIN, OUTPUT)
#define cmt_spi3_sclk_1()	 digitalWrite(CMT2300A_SPI_SCLK_PIN,HIGH)
#define cmt_spi3_sclk_0()	 digitalWrite(CMT2300A_SPI_SCLK_PIN,LOW)
#define cmt_spi3_sdio_1()	 digitalWrite(CMT2300A_SPI_MOSI_PIN,HIGH)
#define cmt_spi3_sdio_0()	 digitalWrite(CMT2300A_SPI_MOSI_PIN,LOW)

#define cmt_spi3_sdio_in()	 pinMode(CMT2300A_SPI_MOSI_PIN, INPUT)
#define cmt_spi3_sdio_out()	 pinMode(CMT2300A_SPI_MOSI_PIN, OUTPUT)
#define cmt_spi3_sdio_read() digitalRead(CMT2300A_SPI_MOSI_PIN)

#define CMT2300A_DelayMs(ms)            delay(ms)
#define CMT2300A_DelayUs(us)            delayMicroseconds(us)
#define g_nSysTickCount 				t_nSysTickCount

/* ************************************************************************ */

void cmt_spi3_init(void);

void cmt_spi3_send(uint8_t data8);
uint8_t cmt_spi3_recv(void);

void cmt_spi3_write(uint8_t addr, uint8_t dat);
void cmt_spi3_read(uint8_t addr, uint8_t* p_dat);

void cmt_spi3_write_fifo(const uint8_t* p_buf, uint16_t len);
void cmt_spi3_read_fifo(uint8_t* p_buf, uint16_t len);

#endif
