#ifndef __PINCONFIG_H
#define __PINCONFIG_H

 /*  Real Board Pin Configuration

            GDO0 - P3.5 (33)
            GDO2 - P3.4 (32)
            CSn  - P3.0 (28)
            SCLK - P3.3 (31)
            SI   - P3.1 (29)
            SO   - P3.2 (30)
 */

 /*  Real Board Pin Configuration

        GDO0 - P1.7 
        GDO2 - P1.3 
        CSn  - P3.0 (28)
        SCLK - P3.3 (31)
        SI   - P3.1 (29)
        SO   - P3.2 (30)
*/
                  //CC1120 Bank 1
    #define RF_GDO0_1                 BIT0
    #define RF_GDO2_1                 BIT1
    #define RF_CSn_1                  BIT7
    #define RF_SI_1                   BIT5
    #define RF_SO_1                   BIT6
    #define RF_SCLK_1                 BIT4
//-------------------------------------------------------------
                  //CC1120 Bank 2
    #define RF_GDO0_2                 BIT3
    #define RF_GDO2_2                 BIT4
    #define RF_CSn_2                  BIT4
    #define RF_SI_2                   BIT2
    #define RF_SO_2                   BIT3
    #define RF_SCLK_2                 BIT1
    
    #define RF_CSn_IN_2               P7IN
    #define RF_CSn_OUT_2              P7OUT
    #define RF_CSn_DIR_2              P7DIR
    #define RF_CSn_SEL_2              P7SEL
//-------------------------------------------------------------
    #define RF_GDO0_IN              P2IN
    #define RF_GDO2_IN              P2IN
    #define RF_CSn_IN               P9IN
    #define RF_SI_IN                P9IN
    #define RF_SO_IN                P9IN
    #define RF_SCLK_IN              P9IN

    #define RF_GDO0_OUT             P2OUT
    #define RF_GDO2_OUT             P2OUT
    #define RF_CSn_OUT              P9OUT
    #define RF_SI_OUT               P9OUT
    #define RF_SO_OUT               P9OUT
    #define RF_SCLK_OUT             P9OUT

    #define RF_GDO0_DIR             P2DIR
    #define RF_GDO2_DIR             P2DIR
    #define RF_CSn_DIR              P9DIR
    #define RF_SI_DIR               P9DIR
    #define RF_SO_DIR               P9DIR
    #define RF_SCLK_DIR             P9DIR

    #define RF_GDO0_SEL             P2SEL
    #define RF_GDO2_SEL             P2SEL
    #define RF_CSn_SEL              P9SEL
    #define RF_SI_SEL               P9SEL
    #define RF_SO_SEL               P9SEL
    #define RF_SCLK_SEL             P9SEL

    #define RF_ANTENNA_DIR          P6DIR
    #define RF_ANTENNA_OUT          P6OUT
    #define RF_ANTENNA1             BIT2
    #define RF_ANTENNA2             BIT3
    #define RF_ANTENNA3             BIT1

    #define RF_RESET1_DIR           P9DIR
    #define RF_RESET1_OUT           P9OUT
    #define RF_RESET1_PIN           BIT0

    #define RF_RESET2_DIR           P7DIR
    #define RF_RESET2_OUT           P7OUT
    #define RF_RESET2_PIN           BIT5

    #define DBG_INIT_BS()               	P7DIR |= ( BIT6 + BIT7)
    #define DBG_INIT_BS1()               	P6DIR |= (BIT4 + BIT7)

    #define DBG_SET_RECEIVE_PIN()               P6OUT |= BIT7
    #define DBG_RESET_RECEIVE_PIN()             P6OUT &= ~BIT7

    #define DBG_SET_SEND_PIN()                  P7OUT |= BIT7
    #define DBG_RESET_SEND_PIN()                P7OUT &= ~BIT7

    #define DBG_TOGGLE_TIMER_PIN()      	P6OUT ^= BIT4
    #define DBG_SET_TIMER_PIN()                 //P2OUT |= BIT0
    #define DBG_RESET_TIMER_PIN()               //P2OUT &= ~BIT0

    #define DBG_SET_UIP_SEND()
    #define DBG_RESET_UIP_SEND()

#endif

