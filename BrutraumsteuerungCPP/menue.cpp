/*
 * Menue.c
 *
 * Created: 12.01.2013 17:42:25
 *  Author: Johannes
 */ 

#include "Main.h"
#include "Menue.h"
#include "hardware.h"

#include "lcd.h"

extern "C" {
#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
};

Display nDisplayPuffer;

void vTasteBearbeiten(const char taste);

enum EDisplay{
	eDisplay_GrundAnz,
	
	eDisplay_Menue1_Soll,
	eDisplay_Menue1_hyster,
	eDisplay_Menue1_cool,
	eDisplay_Menue1_WendeAnzahl,
	eDisplay_Menue1_WendeZeit,
	eDisplay_Menue1_Reset,
	eDisplay_Menue1_Regelparam,
	eDisplay_Menue1_offset,
	eDisplay_Menue1_Enable,
	
	eDisplay_MenueSoll_Raum1,
	eDisplay_MenueSoll_Raum2,
	
	eDisplay_MenueHyster_Raum1,
	eDisplay_MenueHyster_Raum2,
	
	eDisplay_MenueOffset_Raum1,
	eDisplay_MenueOffset_Raum2,
	
	eDisplay_MenueCool_Raum1,
	eDisplay_MenueCool_Raum2,
	
	eDisplay_MenueWendeAnzahl_Raum1,
	eDisplay_MenueWendeAnzahl_Raum2,
	
	eDisplay_MenueWendeZeit_Raum1,
	eDisplay_MenueWendeZeit_Raum2,
	
	eDisplay_MenueEnable_Raum1,
	eDisplay_MenueEnable_Raum2,
	
	eDisplay_MenueRegelparam_Raum1,
	eDisplay_MenueRegelparam_Raum2,
	
	eDisplay_MenueRegelparam_Kp1,
	eDisplay_MenueRegelparam_Ki1,
	eDisplay_MenueRegelparam_Kd1,
	
	eDisplay_MenueRegelparam_Kp2,
	eDisplay_MenueRegelparam_Ki2,
	eDisplay_MenueRegelparam_Kd2,
	
	eDisplay_WertBearbeiten,
	eDisplay_Last
};

char cCurrentDisplay;
char cDisplayMin;
char cDisplayMax;
uint16_t cCurrentWert;


void vUpdateDisplay()
{
	ucUpdateDisplay = 0;
	
	lcd_clear();
	lcd_home();
	
	lcd_string(nDisplayPuffer.z1);
	lcd_setcursor(0,2);
	lcd_string(nDisplayPuffer.z2);
}

void vCheckKeys()
{
	static char ctasteSpeicher = 0;
	
	const char ctaste = (PINC & 0x0F);
	if ((ctaste & (~ctasteSpeicher)) != 0)
	{
		vTasteBearbeiten(ctaste);
	}
	ctasteSpeicher = ctaste;
}

void vWriteChar(char * puf,const char * data)
{
	for(int i= 0; i < 16; ++i)
	{
		puf[i]  = data[i];
		if (data[i] == '\0')
			break;
	}
	puf[16] = '\0';
	ucUpdateDisplay = 1;
}

/* reverse:  reverse string s in place */
void reverse(char s[])
{
	int i, j;
	char c;
	
	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void vDisplayWert(char * puf,uint16_t wWert)
{	
	int i, sign;
	char zwischenpuffer[17]; 
	
	//puf += x;
	
	if ((sign = wWert) < 0)  /* record sign */
		wWert = -wWert;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		zwischenpuffer[i++] = wWert % 10 + '0';   /* get next digit */
	} while ((wWert /= 10) > 0);     /* delete it */
	if (sign < 0)
		zwischenpuffer[i++] = '-';
	zwischenpuffer[i] = '\0';
	reverse(zwischenpuffer);
	
	for(int i = strlen(puf)-1, j = strlen(zwischenpuffer)-1; i >= 0; --i)
	{
		if(puf[i] == '#')
		{
			if ( j >= 0)
			{
				puf[i] = zwischenpuffer[j];
				--j;
			}
			else
			{
				puf[i] = ' ';
			}				
		}
	}
	/*

	puf[i] = puf[i-1];
	puf[i-1] = '.';
	i++;
	puf[i] = '\0';
	
	if(x > 0)
	{
		for(int j  = i+x; j > 0; --j, --i)
		{
			if(i >= 0)
				puf[j] = puf[i];	
			else
				puf[j] = ' '; 
		}
	}
	*/

	ucUpdateDisplay = 1;
}

void vTasteBearbeiten(const char ctaste)
{
	char cAction = eAction_None;
	uint16_t cPara = 0;
	display_active = 1200;
	
	switch(cCurrentDisplay)
	{
	case eDisplay_GrundAnz:
		if(ctaste & KEY_LEFT) cAction = eAction_None; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectHauptMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_None; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_None; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_Soll:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectSollMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_hyster:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectHysterMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_offset:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectOffMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_WendeAnzahl:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectWendeAnzahlMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_WendeZeit:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectWendeZeitMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_Reset:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_ResetParameter; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_Enable:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectEnableMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_cool:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectCoolMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_Menue1_Regelparam:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectRegelparamMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectRegelparamMenue1; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectRegelparamMenue2; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueSoll_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_Sollwert1;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueSoll_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Sollwert2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueHyster_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_Hysterese1;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueHyster_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Hysterese2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueOffset_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_Offset1;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueOffset_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Offset2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueWendeAnzahl_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_WendeAnzahl1;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueWendeAnzahl_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_WendeAnzahl2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueEnable_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_Enable1;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueEnable_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Enable2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueWendeZeit_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_WendeZeit1;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueCool_Raum1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_CoolDown1; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueCool_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) {cAction = eAction_BearbeiteWert; cPara = ePara_CoolDown2;}// taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueWendeZeit_Raum2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectHauptMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_WendeZeit2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Kp1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectRegelparamMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Regelparam_Kp1; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Ki1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectRegelparamMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Regelparam_Ki1; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Kd1:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectRegelparamMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Regelparam_Kd1; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Kp2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectRegelparamMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Regelparam_Kp2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Ki2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectRegelparamMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Regelparam_Ki2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_MenueRegelparam_Kd2:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectRegelparamMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) { cAction = eAction_BearbeiteWert; cPara = ePara_Regelparam_Kd2; } // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_MenueUp; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_MenueDown; // taste 4 down gedrückt
		break;
	case eDisplay_WertBearbeiten:
		if(ctaste & KEY_LEFT) cAction = eAction_BearbeiteEsc; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SpeicherWert; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_WertDown; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_WertUp; // taste 4 down gedrückt
		break;
	default:
	case eDisplay_Last:
		if(ctaste & KEY_LEFT) cAction = eAction_SelectGrundMenue; // taste 1 left gedrückt
		if(ctaste & KEY_RIGHT) cAction = eAction_SelectGrundMenue; // taste 2 right gedrückt
		if(ctaste & KEY_UP) cAction = eAction_SelectGrundMenue; // taste 3 up gedrückt
		if(ctaste & KEY_DOWN) cAction = eAction_SelectGrundMenue; // taste 4 down gedrückt
		break;
		
	}
	
	vActionExec(cAction,cPara);
}

void vActionExec(char cAction,uint16_t cPara)
{		
	switch(cAction)
	{	
	case eAction_None:
		break;
	case eAction_SelectGrundMenue:
		{
			cDisplayMin = eDisplay_GrundAnz;	
			cDisplayMax = eDisplay_GrundAnz;
			cCurrentDisplay = eDisplay_GrundAnz;
		}
		break;
	case eAction_SelectHauptMenue:
		{
			cCurrentDisplay = eDisplay_Menue1_Soll;
			if (u8check_admin())
			{
				cDisplayMin = eDisplay_Menue1_Soll;
				cDisplayMax = eDisplay_Menue1_Enable;
			} 
			else
			{
				cDisplayMin = eDisplay_Menue1_Soll;
				cDisplayMax = eDisplay_Menue1_Reset;
			}
			
		}
		break;
	case eAction_SelectSollMenue:
		{	
			cCurrentDisplay = eDisplay_MenueSoll_Raum1;
			cDisplayMin = eDisplay_MenueSoll_Raum1;
			cDisplayMax = eDisplay_MenueSoll_Raum2;
		}
		break;
	case eAction_SelectHysterMenue:
		{
			cCurrentDisplay = eDisplay_MenueHyster_Raum1;
			cDisplayMin = eDisplay_MenueHyster_Raum1;
			cDisplayMax = eDisplay_MenueHyster_Raum2;
		}
		break;
	case eAction_SelectOffMenue:
		{
			cCurrentDisplay = eDisplay_MenueOffset_Raum1;
			cDisplayMin = eDisplay_MenueOffset_Raum1;
			cDisplayMax = eDisplay_MenueOffset_Raum2;
		}
		break;
	case eAction_SelectCoolMenue:
		{
			cCurrentDisplay = eDisplay_MenueCool_Raum1;
			cDisplayMin = eDisplay_MenueCool_Raum1;
			cDisplayMax = eDisplay_MenueCool_Raum2;
		}
		break;
	case eAction_SelectEnableMenue:
	{
		cCurrentDisplay = eDisplay_MenueEnable_Raum1;
		cDisplayMin = eDisplay_MenueEnable_Raum1;
		cDisplayMax = eDisplay_MenueEnable_Raum2;
	}
	break;
	case eAction_SelectWendeZeitMenue:
		{
			cCurrentDisplay = eDisplay_MenueWendeZeit_Raum1;
			cDisplayMin = eDisplay_MenueWendeZeit_Raum1;
			cDisplayMax = eDisplay_MenueWendeZeit_Raum2;
		}
		break;
	case eAction_SelectWendeAnzahlMenue:
		{
			cCurrentDisplay = eDisplay_MenueWendeAnzahl_Raum1;
			cDisplayMin = eDisplay_MenueWendeAnzahl_Raum1;
			cDisplayMax = eDisplay_MenueWendeAnzahl_Raum2;
		}
		break;
		
	case eAction_SelectRegelparamMenue:
		{
			cCurrentDisplay = eDisplay_MenueRegelparam_Raum1;
			cDisplayMin =  eDisplay_MenueRegelparam_Raum1;;
			cDisplayMax =  eDisplay_MenueRegelparam_Raum2;
		}
		break;
	case eAction_SelectRegelparamMenue1:
		{
			cCurrentDisplay = eDisplay_MenueRegelparam_Kp1;
			cDisplayMin =  eDisplay_MenueRegelparam_Kp1;
			cDisplayMax =  eDisplay_MenueRegelparam_Kd1;
		}
			break;
	case eAction_SelectRegelparamMenue2:
		{
			cCurrentDisplay = eDisplay_MenueRegelparam_Kp2;
			cDisplayMin =  eDisplay_MenueRegelparam_Kp2;
			cDisplayMax =  eDisplay_MenueRegelparam_Kd2;
		}
			break;
	case eAction_MenueUp:
		{
			cCurrentDisplay++;
			if (cCurrentDisplay > cDisplayMax)
				cCurrentDisplay = cDisplayMin;
		}		
		break;
	case eAction_MenueDown:
		{
			cCurrentDisplay--;
			if (cCurrentDisplay < cDisplayMin)
				cCurrentDisplay = cDisplayMax;
		}
		break;
	case eAction_BearbeiteWert:
		{
			cCurrentWert = cPara;
									
			cCurrentDisplay = eDisplay_WertBearbeiten;
			cDisplayMin = eDisplay_WertBearbeiten;
			cDisplayMax = eDisplay_WertBearbeiten;
		}
		break;
	case eAction_BearbeiteEsc:
		{
			pwParameter[cCurrentWert] = pwParameterSave[cCurrentWert];
			switch(cCurrentWert)
			{
			case ePara_Sollwert1:
			case ePara_Sollwert2: 
				vActionExec(eAction_SelectSollMenue,0);
				break;	
			case ePara_Hysterese1:
			case ePara_Hysterese2:
				vActionExec(eAction_SelectHysterMenue,0);
				break;
			case ePara_Offset1:
			case ePara_Offset2:
				vActionExec(eAction_SelectOffMenue,0);
				break;
			case ePara_CoolDown1:
			case ePara_CoolDown2:
				vActionExec(eAction_SelectCoolMenue,0);
				break;
			case ePara_Enable1:
			case ePara_Enable2:
				vActionExec(eAction_SelectEnableMenue,0);
				break;
			case ePara_WendeAnzahl1:
			case ePara_WendeAnzahl2:
				vActionExec(eAction_SelectWendeAnzahlMenue,0);
				break;
			case ePara_WendeZeit1:
			case ePara_WendeZeit2:
				vActionExec(eAction_SelectWendeZeitMenue,0);
				break;	
			case ePara_Regelparam_Kp1:
			case ePara_Regelparam_Ki1:
			case ePara_Regelparam_Kd1:
				vActionExec(eAction_SelectRegelparamMenue1,0);
				break;
			case ePara_Regelparam_Kp2:
			case ePara_Regelparam_Ki2:
			case ePara_Regelparam_Kd2:
				vActionExec(eAction_SelectRegelparamMenue2,0);
				break;
			}			
		}
		break;
	case eAction_ResetParameter:
		{
			vSetSound_ms(100);
			for(int i = 0; i < ePara_Last; ++i)
			{
				vSaveParaWert(i,pwParameterDef[i]);
			}
			break;
		}
	case eAction_SpeicherWert:
		{
			vSetSound_ms(5);
			vSavePara(cCurrentWert);
		}		
		break;
	case eAction_WertUp:
		{
			if(pwParameter[cCurrentWert] < pwParameterMax[cCurrentWert])
				pwParameter[cCurrentWert]++;
		}
		break;
	case eAction_WertDown:
		{
			if(pwParameter[cCurrentWert] > pwParameterMin[cCurrentWert])
				pwParameter[cCurrentWert]--;
		
		}
		break;
	}
	
	vAnzeigeBearbeiten();
}

void vAnzeigeBearbeiten()
{	
	for(int i = 0; i < 16; ++i)
	{
		nDisplayPuffer.z1[i] = ' ';
		nDisplayPuffer.z2[i] = ' ';
	}
	switch(cCurrentDisplay)
	{
	case eDisplay_GrundAnz:
		if (disp == 0)
		{
			switch(t_param1.status_ist)
			{
				case eHeat_On:	
					vWriteChar(nDisplayPuffer.z1,"(1) Temp: ##.#\xdf\C "); 
					set_status(eStatus_OK);
					break;
				case eHeat_Cooldown: 
					vWriteChar(nDisplayPuffer.z1,"(1)Temp: ##.#\xdf\C \x2A"); 
					set_status(eStatus_OK);
					break;
				case eHeat_SensorFail:
					vWriteChar(nDisplayPuffer.z1,"(1)Sens Def##.#\xdf\C");
					set_status(eStatus_Failure);
					break;
				case eHeat_NA:
					vWriteChar(nDisplayPuffer.z1,"(1)no support");
					set_status(eStatus_Failure);
					break;
				case eHeat_Overtemp:
					vWriteChar(nDisplayPuffer.z1,"(1)zu warm:##.#\xdf\C ");
					set_status(eStatus_Warning);
					break;
				case eHeat_CriticalOvertemp:
					vWriteChar(nDisplayPuffer.z1,"(1)BRANDGEFAHR!");
					set_status(eStatus_Failure);
					break;
				case eHeat_Off:
					vWriteChar(nDisplayPuffer.z1,"(1)Deaktiviert");
					set_status(eStatus_Off);
					break;
			}
			vWriteChar(nDisplayPuffer.z2," Feuchte: ##.#%");
			vDisplayWert(nDisplayPuffer.z1,Temp1);
			vDisplayWert(nDisplayPuffer.z2,(uint16_t)(Hygrometer_1.f_humidity * 10));
			
		} 
		else if(disp == 1)
		{
			switch(t_param2.status_ist)
			{
				case eHeat_On:	
					vWriteChar(nDisplayPuffer.z1,"(2) Temp: ##.#\xdf\C "); 
					set_status(eStatus_OK);
					break;
				case eHeat_Cooldown: 
					vWriteChar(nDisplayPuffer.z1,"(2)Temp: ##.#\xdf\C \x2A"); 
					set_status(eStatus_OK);
					break;
				case eHeat_SensorFail:
					vWriteChar(nDisplayPuffer.z1,"(2)Sens Def##.#\xdf\C");
					set_status(eStatus_Failure);
					break;
				case eHeat_NA:
					vWriteChar(nDisplayPuffer.z1,"(2)no support");
					set_status(eStatus_Failure);
					break;
				case eHeat_Overtemp:
					vWriteChar(nDisplayPuffer.z1,"(2)zu warm:##.#\xdf\C ");
					set_status(eStatus_Warning);
					break;
				case eHeat_CriticalOvertemp:
					vWriteChar(nDisplayPuffer.z1,"(2)BRANDGEFAHR!");
					set_status(eStatus_Failure);
					break;
				case eHeat_Off:
					vWriteChar(nDisplayPuffer.z1,"(2)Deaktiviert");
					set_status(eStatus_Off);
					break;
			}
			vWriteChar(nDisplayPuffer.z2," Feuchte: ##.#%");
			vDisplayWert(nDisplayPuffer.z1,Temp2);
			vDisplayWert(nDisplayPuffer.z2,(uint16_t)(Hygrometer_2.f_humidity * 10));
		}
		/*		
		nDisplayPuffer.z1[15] = 1;
		nDisplayPuffer.z2[15] = 1;
		nDisplayPuffer.z1[12] = 0xDF;
		nDisplayPuffer.z2[12] = 0xDF;
		nDisplayPuffer.z1[13] = 'C';
		nDisplayPuffer.z2[13] = 'C';
		*/
		break;
	case eDisplay_Menue1_Soll:
		vWriteChar(nDisplayPuffer.z1,"Sollwert");
		vWriteChar(nDisplayPuffer.z2,"Temperatur");
		break;
	case eDisplay_Menue1_hyster:
		vWriteChar(nDisplayPuffer.z1,"Schalthysterese");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_Menue1_offset:
		vWriteChar(nDisplayPuffer.z1,"Sensor");
		vWriteChar(nDisplayPuffer.z2,"Kalibrierung");
		break;
	case eDisplay_Menue1_cool:
		vWriteChar(nDisplayPuffer.z1,"Abk\xf5hl");
		vWriteChar(nDisplayPuffer.z2,"Phasen");
		break;
	case eDisplay_Menue1_WendeAnzahl:
		vWriteChar(nDisplayPuffer.z1,"Wendezahl");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_Menue1_WendeZeit:
		vWriteChar(nDisplayPuffer.z1,"Wendedauer");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_Menue1_Enable:
		vWriteChar(nDisplayPuffer.z1,"R\xe1ume");
		vWriteChar(nDisplayPuffer.z2,"EIN / AUS");
		break;
	case eDisplay_Menue1_Reset:
		vWriteChar(nDisplayPuffer.z1,"Zur\xf5\cksetzen");
		vWriteChar(nDisplayPuffer.z2,"\x7f ESC       OK \x7E");
		break;
	case eDisplay_Menue1_Regelparam:
		vWriteChar(nDisplayPuffer.z1,"Regelparameter");
		vWriteChar(nDisplayPuffer.z2,"");
		break;	
	case eDisplay_MenueRegelparam_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Regelparameter");
		vWriteChar(nDisplayPuffer.z2,"Raum           1");
		break;
	case eDisplay_MenueRegelparam_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Regelparameter");
		vWriteChar(nDisplayPuffer.z2,"Raum           2");
		break;
	case eDisplay_MenueSoll_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Soll Raum 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueSoll_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Soll Raum 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueHyster_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Hysterese 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueHyster_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Hysterese 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueOffset_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Offset 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueOffset_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Offset 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueCool_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Cool 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueCool_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Cool 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueWendeAnzahl_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Wendezahl 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueWendeAnzahl_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Wendezahl 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueEnable_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Raum 1");
		vWriteChar(nDisplayPuffer.z2,"EIN / AUS");
		break;
	case eDisplay_MenueEnable_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Raum 2");
		vWriteChar(nDisplayPuffer.z2,"EIN / AUS");
		break;
	case eDisplay_MenueWendeZeit_Raum1:
		vWriteChar(nDisplayPuffer.z1,"Wendezeit 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueWendeZeit_Raum2:
		vWriteChar(nDisplayPuffer.z1,"Wendezeit 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueRegelparam_Kp1:
		vWriteChar(nDisplayPuffer.z1,"Regelparam. Kp 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueRegelparam_Ki1:
		vWriteChar(nDisplayPuffer.z1,"Regelparam. Ki 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueRegelparam_Kd1:
		vWriteChar(nDisplayPuffer.z1,"Regelparam. Kd 1");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueRegelparam_Kp2:
		vWriteChar(nDisplayPuffer.z1,"Regelparam. Kp 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueRegelparam_Ki2:
		vWriteChar(nDisplayPuffer.z1,"Regelparam. Ki 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_MenueRegelparam_Kd2:
		vWriteChar(nDisplayPuffer.z1,"Regelparam. Kd 2");
		vWriteChar(nDisplayPuffer.z2,"");
		break;
	case eDisplay_WertBearbeiten:
		switch(cCurrentWert)
		{														//  |..............     .|
			case ePara_Sollwert1: vWriteChar(nDisplayPuffer.z1,		"T_soll 1: ##.#\xdf\C"); break;
			case ePara_Sollwert2: vWriteChar(nDisplayPuffer.z1,		"T_soll 2: ##.#\xdf\C"); break;
			case ePara_Offset1: vWriteChar(nDisplayPuffer.z1,		"Offset 1: ##.#\xdf\C"); break;
			case ePara_Offset2: vWriteChar(nDisplayPuffer.z1,		"Offset 2: ##.#\xdf\C"); break;
			case ePara_Hysterese1: vWriteChar(nDisplayPuffer.z1,	"Hyst. 1:  ##.#\xdf\C"); break;
			case ePara_Hysterese2: vWriteChar(nDisplayPuffer.z1,	"Hyst. 2:  ##.#\xdf\C"); break;
			case ePara_CoolDown1: vWriteChar(nDisplayPuffer.z1,		"Cool 1:  ###min"); break;
			case ePara_CoolDown2: vWriteChar(nDisplayPuffer.z1,		"Cool 2:  ###min"); break;
			case ePara_Enable1: vWriteChar(nDisplayPuffer.z1,		"Raum 1:     ###"); break;
			case ePara_Enable2: vWriteChar(nDisplayPuffer.z1,		"Raum 2:     ###"); break;
			case ePara_WendeAnzahl1: vWriteChar(nDisplayPuffer.z1,	"Wenden 1: #/day"); break;
			case ePara_WendeAnzahl2: vWriteChar(nDisplayPuffer.z1,	"Wenden 2: #/day"); break;
			case ePara_WendeZeit1: vWriteChar(nDisplayPuffer.z1,	"Dauer 1:     ##s"); break;
			case ePara_WendeZeit2: vWriteChar(nDisplayPuffer.z1,	"Dauer 2:     ##s"); break;
			case ePara_Regelparam_Kp1: vWriteChar(nDisplayPuffer.z1,	"Kp 1:      ##.##"); break;
			case ePara_Regelparam_Ki1: vWriteChar(nDisplayPuffer.z1,	"Ki 1:      ##.##"); break;
			case ePara_Regelparam_Kd1: vWriteChar(nDisplayPuffer.z1,	"Kd 1:      ##.##"); break;
			case ePara_Regelparam_Kp2: vWriteChar(nDisplayPuffer.z1,	"Kp 2:      ##.##"); break;
			case ePara_Regelparam_Ki2: vWriteChar(nDisplayPuffer.z1,	"Ki 2:      ##.##"); break;
			case ePara_Regelparam_Kd2: vWriteChar(nDisplayPuffer.z1,	"Kd 2:      ##.##"); break;
		}
		vDisplayWert(nDisplayPuffer.z1,pwParameter[cCurrentWert]);
		
		if(pwParameter[cCurrentWert] == pwParameterSave[cCurrentWert])
		{
			vWriteChar(nDisplayPuffer.z2,"\x7f ESC");
		}
		else
		{
			vWriteChar(nDisplayPuffer.z2,"\x7f ESC     SAVE \x7E");			
		}
		break;		
	default:
	case eDisplay_Last:
		vWriteChar(nDisplayPuffer.z1,"Fehler");
		vWriteChar(nDisplayPuffer.z2,"Text fehlt");
		break;		
	}
	
	nDisplayPuffer.z1[16] = 0;
	nDisplayPuffer.z2[16] = 0;
}
