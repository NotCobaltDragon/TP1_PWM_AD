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


S_pwmSettings PWMData;      // pour les settings
APP_DATA appData;

void GPWM_Initialize(S_pwmSettings *pData)
{
   //Init datas 
    pData -> AngleSetting = 0;
    pData -> SpeedSetting = 0;
    pData -> absAngle = 0;
    pData -> absSpeed = 0;
    
   // Init état du pont en H
    BSP_EnableHbrige();
    
   // lance les timers et OC
   DRV_TMR0_Start();    //Initialisation du timer 1
   DRV_TMR1_Start();    //Initialisation du timer 2
   DRV_TMR2_Start();    //Initialisation du timer 3
   DRV_TMR3_Start();    //Initialisation du timer 4
   
   DRV_OC0_Start();      //Initilisation de l'OC 2
   DRV_OC1_Start();      //Initilisation de l'OC 3
   
   STBY_HBRIDGE_W = 1;  //Pont en H Standby toujours à 1
    
}

// Obtention vitesse et angle (mise a jour des 4 champs de la structure)
void GPWM_GetSettings(S_pwmSettings *pData)	
{
   /*S_ADCResults Res;
    
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
    pData ->AngleSetting = pData ->absAngle - 90;*/
    
    uint16_t Val_moyenneADC_CH0, Val_moyenneADC_CH1;
    uint8_t Val_Conv;
    static uint16_t Tab_moy_CH0[TAILLE_MOYENNE] = {0};
    
    static uint16_t Tab_moy_CH1[TAILLE_MOYENNE] = {0}; 
    
   
    /******---Traitement 1er AD---***********/
    // Lecture du convertisseur AD-CH0
    appData.adcRes = BSP_ReadAllADC();//Lecture des pot
    Tab_moy_CH0[0] = appData.adcRes.Chan0;
    Tab_moy_CH1[0] = appData.adcRes.Chan1;
    
    Val_moyenneADC_CH0 = ValADC_MOY_CH(Tab_moy_CH0); //Appel de fonction pour avoir une moyenne glissante sur les 10 dernières valeurs
    
    // conversion
    Val_Conv = (Val_moyenneADC_CH0*198)/1023; //Valeur brute de 0 à 198
    pData->SpeedSetting = Val_Conv - 99; //Valeurs signée -99 à 99
    
    if(pData->SpeedSetting < 0)
    {
        pData->absSpeed = pData->SpeedSetting * -1; //Conversion val négatif à positive
    }
    else
    {
        pData->absSpeed = pData->SpeedSetting;
    }
    /******---Traitement 2ème AD---***********/
    // Lecture du convertisseur AD-CH1
    Val_moyenneADC_CH1 = ValADC_MOY_CH(Tab_moy_CH1); //Appel de fonction pour avoir une moyenne glissante sur les 10 dernières valeurs   
   
    // conversion
    pData->absAngle = (Val_moyenneADC_CH1*180)/1023; //Valeur brute de 0 à 180
    pData->AngleSetting = pData->absAngle - 90; //Valeurs signée -90 à 90   
}


// Affichage des information en exploitant la structure
void GPWM_DispSettings(S_pwmSettings *pData)
{
    //lcd_gotoxy(1, 1);
    //printf_lcd("TP1 PWM 2022-2023");
    
    lcd_gotoxy(1, 2);
    if(pData -> SpeedSetting >= 0)
    {
        printf_lcd("Speed Setting   +%02d", pData -> SpeedSetting );
    }
    else
    {
        printf_lcd("Speed Setting   %03d", pData -> SpeedSetting );
    }
    
    lcd_gotoxy(1, 3);
    printf_lcd("Absolute Speed   %02d", pData -> absSpeed ); 

    lcd_gotoxy(1, 4);
    printf_lcd("Angle           %03d", pData -> absAngle ); 
}

// Execution PWM et gestion moteur à partir des info dans structure
void GPWM_ExecPWM(S_pwmSettings *pData)
{
    if(pData->SpeedSetting < 0) //On vient changer le sens du moteur
    {
        //Sens anti-horaire
        AIN1_HBRIDGE_W = 0;
        AIN2_HBRIDGE_W = 1;
    }
    else
    {
        //Sens horaire
        AIN1_HBRIDGE_W = 1;
        AIN2_HBRIDGE_W = 0;
    }
    
    //Reglage largeur impulsion moteur DC
    appData.PulseWidthOC2 = ((pData->absSpeed*249) /99);
    PLIB_OC_PulseWidth16BitSet(OC_ID_2, appData.PulseWidthOC2);
    
    //Reglage largeur impulsion servomoteur
    appData.PulseWidthOC3 = (pData->absAngle * 2250 / 180) + 750;
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
    for(i = 0; i < TAILLE_MOYENNE; i++)
    {
        ValTotal = Tabl_MOY[9-i] + ValTotal;
        if(i < 9)
        {
          Tabl_MOY[9-i] = Tabl_MOY[8-i];
        }
    }
    
    return(ValTotal/TAILLE_MOYENNE);
}
