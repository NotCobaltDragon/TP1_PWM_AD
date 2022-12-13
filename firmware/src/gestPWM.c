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
#include "bsp.h"

S_pwmSettings PWMData;      // pour les settings

void GPWM_Initialize(S_pwmSettings *pData)
{
   // Init les data 
    
   // Init état du pont en H
    BSP_EnableHbrige();
    
   // lance les timers et OC
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
   S_ADCResults Res;
    
    int TableAverage0[10];
    int TableAverage1[10];
    
    char ImoyG0;
    char ImoyG1;
    
    char Valeur0to198;
    
    const float VALEUR_ANGLE_MAX = 1023;
    const float VALEUR_BRUTE_AD_MAX = 180;
    const float VALEUR_NON_SIGNEE_MAX = 198;
    
    // initialise le convertisseur AD
    
    BSP_InitADC10();
    
    // lecture du canal 0
    
    for (ImoyG0 = 0; ImoyG0 < 10; ImoyG0++)
    {
        Res = BSP_ReadAllADC();
    
        TableAverage0[ImoyG0] = Res.Chan0;
    }
 
    // lecture du canal 1
    
    for (ImoyG1 = 0; ImoyG1 < 10; ImoyG1++)
    {
        Res = BSP_ReadAllADC();
    
        TableAverage1[ImoyG1] = Res.Chan1;
    }
    
    // clear le canal 0
    
    Res.Chan0 = 0;
    
    // clear le canal 1
    
    Res.Chan1 = 0;
    
    // moyenne glissante canal 0
    
    for (ImoyG0 = 0; ImoyG0 < 10; ImoyG0++)
    {
        Res.Chan0 = Res.Chan0 + TableAverage0[ImoyG0]; 
    }
    
    Res.Chan0 = Res.Chan0 / 10;
    
    // moyenne glissante canal 1
    
    for (ImoyG1 = 0; ImoyG1 < 10; ImoyG1++)
    {
        Res.Chan1 = Res.Chan1 + TableAverage1[ImoyG1]; 
    }
    
    Res.Chan1 = Res.Chan1 / 10;
    
    
    // Information speed et absSpeed
    
    Valeur0to198 = Res.Chan0 * (VALEUR_BRUTE_AD_MAX / VALEUR_NON_SIGNEE_MAX);
    pData ->SpeedSetting = Valeur0to198 - 99; 
    pData ->absSpeed = Valeur0to198 - 99;
    
    // Information absAngle, AngleSetting
    
    pData ->absAngle =  Res.Chan1 * (VALEUR_BRUTE_AD_MAX / VALEUR_ANGLE_MAX);
    pData ->AngleSetting = pData ->absAngle - 90;
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    lcd_gotoxy(1,1);
    printf_lcd("TP1 PWM 2022-2023");
  
    lcd_gotoxy(1,2);
    printf_lcd("SpeedSetting %03d",pData ->SpeedSetting );
    
    lcd_gotoxy(1,3);
    printf_lcd("AbsSpeed %02d",pData ->absSpeed ); 

    lcd_gotoxy(1,3);
    printf_lcd("Angle %03d",pData ->absAngle ); 
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    
}

// Execution PWM software
void GPWM_ExecPWMSoft(S_pwmSettings *pData)
{
    
}


