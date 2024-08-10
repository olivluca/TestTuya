/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */

#include "radio.h"
#include "cmt2300a_hal.h"
#include "cmt2300a_params_captured.h"

int RF_Init(void)
{
    uint8_t tmp;
    
    CMT2300A_InitGpio();
	CMT2300A_Init();
    
    /* Config registers */
    CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR       , g_cmt2300aCmtBank       , CMT2300A_CMT_BANK_SIZE       );
    CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR    , g_cmt2300aSystemBank    , CMT2300A_SYSTEM_BANK_SIZE    );
    CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , g_cmt2300aFrequencyBank , CMT2300A_FREQUENCY_BANK_SIZE );
    CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , g_cmt2300aDataRateBank  , CMT2300A_DATA_RATE_BANK_SIZE );
    CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR  , g_cmt2300aBasebandBank  , CMT2300A_BASEBAND_BANK_SIZE  );
    CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , g_cmt2300aTxBank        , CMT2300A_TX_BANK_SIZE        );
    
    // xosc_aac_code[2:0] = 2
    tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
    CMT2300A_WriteReg(CMT2300A_CUS_CMT10, tmp|0x02);
    
	if(false==CMT2300A_IsExist()) 
	{
        //CMT2300A not found!
        return -1;
    }
    else
	{
        return 0;
    }
}

int StartTx() {
 	pinMode(CMT2300A_GPIO1_PIN, OUTPUT);
	digitalWrite(CMT2300A_GPIO1_PIN, LOW);
     if (RF_Init()!=0) {
        return 1;
	}
    CMT2300A_WriteReg(CMT2300A_CUS_SYS2,0); //???? 
    CMT2300A_ConfigGpio(CMT2300A_GPIO1_SEL_DOUT | CMT2300A_GPIO3_SEL_DIN | CMT2300A_GPIO2_SEL_INT2);
	CMT2300A_EnableTxDin(true);    
	CMT2300A_ConfigTxDin(CMT2300A_TX_DIN_SEL_GPIO1);
	CMT2300A_EnableTxDinInvert(false); 
	CMT2300A_GoSleep();
	CMT2300A_GoStby();
	if (CMT2300A_GoTx()) {
  	    digitalWrite(CMT2300A_GPIO1_PIN, HIGH);
        return 0;
    } else {
        return 2;
    }
}
 

int StartRx() {
    if (RF_Init()!=0) {
        return 1;
	}

    CMT2300A_ConfigGpio (CMT2300A_GPIO1_SEL_INT1 | CMT2300A_GPIO2_SEL_INT2 | CMT2300A_GPIO3_SEL_DOUT);
	CMT2300A_ConfigInterrupt(CMT2300A_INT_SEL_TX_DONE, CMT2300A_INT_SEL_PKT_OK);
	CMT2300A_EnableInterrupt(CMT2300A_MASK_TX_DONE_EN | CMT2300A_MASK_PKT_DONE_EN);

	CMT2300A_WriteReg(CMT2300A_CUS_SYS2 , 0);

	CMT2300A_EnableFifoMerge(true);

	CMT2300A_WriteReg(CMT2300A_CUS_PKT29, 0x20); 

	CMT2300A_GoSleep();
	CMT2300A_GoStby();
	CMT2300A_ConfigGpio (CMT2300A_GPIO1_SEL_DCLK | CMT2300A_GPIO2_SEL_DOUT | CMT2300A_GPIO3_SEL_INT2);
	CMT2300A_ConfigInterrupt(CMT2300A_INT_SEL_SYNC_OK | CMT2300A_INT_SEL_SL_TMO, CMT2300A_INT_SEL_PKT_OK);
    CMT2300A_EnableFifoMerge(true);
	CMT2300A_ClearInterruptFlags();
	CMT2300A_ClearRxFifo();
	pinMode(CMT2300A_GPIO2_PIN, INPUT);
	
    CMT2300A_GoRx();

	//CMT2300_IsExist()
	int rssi=CMT2300A_GetRssiDBm();
	//stream->print("rssi dbm ");
	//stream->println(rssi);

	CMT2300A_ClearInterruptFlags();
    return 0;
}