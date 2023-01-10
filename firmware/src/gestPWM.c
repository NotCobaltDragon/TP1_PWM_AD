/*--------------------------------------------------------*/
// GestPWM.c
/*--------------------------------------------------------*/
//	Description :	Gestion des PWM 
//			        pour TP1 2022-2023
//
//	Auteur 		: 	A. Steffen & J. Chafla
//
//	Version		:	V1.1
//	Compilateur	:	XC32 V5.45 + Harmony 2.06
//
/*--------------------------------------------------------*/



#include "GestPWM.h"


S_pwmSettings PWMData;  //For settings value
APP_DATA appData;

void GPWM_Initialize(S_pwmSettings *pData)
{
   //Init datas 
    pData -> AngleSetting = 0;
    pData -> SpeedSetting = 0;
    pData -> absAngle = 0;
    pData -> absSpeed = 0;
    
    //Init H Bridge
    BSP_EnableHbrige();
    
   //Init timers 1 to 4
   DRV_TMR0_Start();
   DRV_TMR1_Start();
   DRV_TMR2_Start();
   DRV_TMR3_Start();
   
   //Init OC 2 and 3
   DRV_OC0_Start();
   DRV_OC1_Start(); 
   
   STBY_HBRIDGE_W = 1;  //H Bridge in standby, always at 1
}

// Get values for speed and angle 
void GPWM_GetSettings(S_pwmSettings *pData)	
{   
    static uint16_t Table_Avg_CH0[AVERAGE_SIZE];    //Table for rolling average
    
    static uint16_t Table_Avg_CH1[AVERAGE_SIZE];    //Table for rolling average

    uint16_t Avg_ADC_CH0, Avg_ADC_CH1;  //Output average values 
    uint8_t Val_Conv;
    
    /******---Conversion of ADC, Channel 0---***********/
    
    // Lecture du convertisseur AD-CH0
    appData.adcRes = BSP_ReadAllADC();  //Read all potentiometers
    Table_Avg_CH0[0] = appData.adcRes.Chan0;
    Table_Avg_CH1[0] = appData.adcRes.Chan1;
    
    Avg_ADC_CH0 = ValADC_MOY_CH(Table_Avg_CH0); //Call function to get a rolling average

    // conversion
    Val_Conv = (Avg_ADC_CH0*198)/1023;  //Raw value from 0 to 198
    pData->SpeedSetting = Val_Conv - 99;    //Signed value from -99 to 99
    
    if(pData->SpeedSetting < 0)
    {
        pData->absSpeed = pData->SpeedSetting * -1; //Convert negative value to positive
    }
    else
    {
        pData->absSpeed = pData->SpeedSetting;
    }

    /******---Conversion of ADC, Channel 1---***********/

    // Lecture du convertisseur AD-CH1
    Avg_ADC_CH1 = ValADC_MOY_CH(Table_Avg_CH1); //Call function to get a rolling average  
   
    // conversion
    pData->absAngle = (Avg_ADC_CH1*180)/1023;    //Raw value from 0 to 180
    pData->AngleSetting = pData->absAngle - 90;     //Signed value from -90 to 90   
}

void GPWM_DispSettings(S_pwmSettings *pData) //Display settings on LCD
{
    lcd_gotoxy(1, 2);
    if(pData -> SpeedSetting >= 0)  //Check if value is positive/negative
    {
        printf_lcd("Speed Setting   +%2d", pData -> SpeedSetting );
    }
    else
    {
        printf_lcd("Speed Setting   %3d", pData -> SpeedSetting );
    }
    
    lcd_gotoxy(1, 3);
    printf_lcd("Absolute Speed   %2d", pData -> absSpeed ); 

    lcd_gotoxy(1, 4);
    if(pData -> AngleSetting >= 0)  //Check if value is positive/negative
    {
        printf_lcd("Angle           +%2d", pData -> AngleSetting );
    }
    else
    {
        printf_lcd("Angle           %3d", pData -> AngleSetting );
    }
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    if(pData->SpeedSetting > 0)
    {
        STBY_HBRIDGE_W = 1;
        AIN1_HBRIDGE_W = 1;
        AIN2_HBRIDGE_W = 0;
    }
    else if(pData->SpeedSetting == 0)
    {
        STBY_HBRIDGE_W = 0;
    }
    else
    {
        STBY_HBRIDGE_W = 1;
        AIN1_HBRIDGE_W = 0;
        AIN2_HBRIDGE_W = 1;
    }
    
    //Reglage largeur impulsion moteur DC
    appData.PulseWidthOC2 = ((pData->absSpeed*1999)/99); //old *249)/99
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, appData.PulseWidthOC2);
    
    //Reglage largeur impulsion servomoteur
    appData.PulseWidthOC3 = (pData->absAngle * ((13499 - 2999)/180) + 2999) ; //  //old * 2250 / 180) + 750
    PLIB_OC_PulseWidth16BitSet(OC_ID_3, appData.PulseWidthOC3);    
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    static char Counter = 0;
    static char Counter2 = 0;
     
    if(Counter > 99)
    {
        Counter2 = pData->absSpeed;
        Counter = 0;
    }    

    if(Counter2 <= Counter)
    {
        BSP_LEDOn(BSP_LED_2);
    }
    else
    {
        BSP_LEDOff(BSP_LED_2);
    }
    Counter++; 
}




uint16_t ValADC_MOY_CH(uint16_t Tabl_MOY[])
{ 
    uint32_t ValTotal = 0;
    uint8_t i = 0;
    
    //remplissage des 10 cases du tableau
    for(i = 0; i < AVERAGE_SIZE; i++)
    {
        ValTotal = Tabl_MOY[9-i] + ValTotal;
        if(i < 9)
        {
          Tabl_MOY[9-i] = Tabl_MOY[8-i];
        }
    }
    
    return(ValTotal/AVERAGE_SIZE);
}
