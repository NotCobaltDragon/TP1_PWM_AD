#ifndef GESTPWM_H
#define GESTPWM_H
/*--------------------------------------------------------*/
// GestPWM.h
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

#include <stdint.h>

#include "app.h"

#include "system_config.h"
#include "system_definitions.h"
#include "bsp.h"

#include "Mc32DriverAdc.h"
#include "Mc32DriverLcd.h"
#include "peripheral/oc/plib_oc.h"


#define PERIODEMAX 1500
#define PERIODEMIN 375
#define PERIODEMILLIEU 1125             // = PERIODEMAX - PERIODEMIN
#define TAILLE_MOYENNE 10

/*--------------------------------------------------------*/
// Function Prototype
/*--------------------------------------------------------*/

typedef struct {
    uint8_t absSpeed;    // Absolute Speed 0 to 99
    uint8_t absAngle;    // Angle  0 to 180°
    int8_t SpeedSetting; // Defined Speed -99 à +99
    int8_t AngleSetting; // Defined Angle  -90° à +90°
} S_pwmSettings;

extern S_pwmSettings PWMData; 

void GPWM_Initialize(S_pwmSettings *pData);

// Ces 3 fonctions ont pour paramètre un pointeur sur la structure S_pwmSettings.
void GPWM_GetSettings(S_pwmSettings *pData);         // Obtention vitesse et angle
void GPWM_DispSettings(S_pwmSettings *pData);        // Affichage
void GPWM_ExecPWM(S_pwmSettings *pData);             // Execution PWM et gestion moteur.
void GPWM_ExecPWMSoft(S_pwmSettings *pData);    // Execution PWM software.

uint16_t ValADC_MOY_CH(uint16_t Tabl_MOY[]);

#endif
