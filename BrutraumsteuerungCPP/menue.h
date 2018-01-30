/*
 * Menue.h
 *
 * Created: 12.01.2013 17:42:53
 *  Author: Johannes
 */ 


#ifndef MENUE_H_
#define MENUE_H_

#define KEY_UP		0x01
#define KEY_DOWN	0x04
#define KEY_LEFT	0x02
#define KEY_RIGHT	0x08

void vCheckKeys();
void vAnzeigeBearbeiten();
void vActionExec(char eAction,uint16_t cPara);

enum EAaktion {
	eAction_None,
	eAction_SelectGrundMenue,
	eAction_SelectHauptMenue,
	eAction_SelectSollMenue,
	eAction_SelectHysterMenue,
	eAction_SelectOffMenue,
	eAction_SelectCoolMenue,
	eAction_SelectWendeZeitMenue,
	eAction_SelectRegelparamMenue,
	eAction_SelectRegelparamMenue1,
	eAction_SelectRegelparamMenue2,
	eAction_SelectWendeAnzahlMenue,
	eAction_SelectEnableMenue,
	eAction_ResetParameter,
	eAction_BearbeiteWert,
	eAction_BearbeiteEsc,
	eAction_SpeicherWert,
	eAction_MenueUp,
	eAction_MenueDown,
	eAction_WertUp,
	eAction_WertDown
};

typedef struct {
	char z1[17];
	char z2[17];
} Display;
void vWriteChar(char * puf,const char * data);
void vDisplayWert(char * puf,uint16_t cWert);

extern Display nDisplayPuffer;


#endif /* MENUE_H_ */