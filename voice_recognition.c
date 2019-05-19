/* ----------------------------------------------------------------------
FILE : prog.c
PROJECT : Robot commandé par la voix - Télécommande
AUTHORS : MACIA Jimmy & GRAUSO Maxime
CREATED ON : April 12th 2019
VERSION : 1.0
---------------------------------------------------------------------- */

#include <techlib.h>
#include "demo_io.h"
#include "recog\comm_rscApp_commande_robot.h" 

#define OUTPUT_ADR 14

int adr0, adr1, adr2; // Les adresses des ports de sortie de la télécommande

short btnFlag = 0;

short autoFlag = 0; // Permet de d�terminer si le bouton a �t� press�
short autoVoice = 0; // Le mode d�tection de voix auto est-il actif ?

PARAMETERPASS results;

void set_signal (int _0, int _1, int _2) {
	// Modifie la tension au sorties spécifiées
	if(_0 == 0) _LowPin(adr0); else _HighPin(adr0);
	if(_1 == 0) _LowPin(adr1); else _HighPin(adr1);
	if(_2 == 0) _LowPin(adr2); else _HighPin(adr2);
}

void clignoter (int _nbr, int _delay) {
	// Fait clignoter la diode VERTE le nombre de fois spécifié
	int i = 0;
	_GreenOff();
	while (i < _nbr) {
		_DelayMilliSecs(_delay);
		_GreenOn();
		_DelayMilliSecs(_delay);
		_GreenOff();
		i++;
	}
	_GreenOn();
}

void erreur () {
	_RedOn();
	_DelayMilliSecs(2000);
	_RedOff();
}

void ordre () {

	uchar error;
	uchar class;
	
// On passe en mode "Écoute" : les ordres sont détectés ici

	_SysBeep(20);
	_YellowOn();
	error = _T2SI((long)&nn_commande_robot, (long)&gsearch_comm_commande_robot, T2SI_DEFAULT_KNOB, T2SI_DEFAULT_TIMEOUT, T2SI_DEFAULT_TRAILING, &results);
	class = results.pp_b;
	_YellowOff();

	if (error == ERR_RECOG_LOW_CONF) { // Erreur : allume le voyant ROUGE un moment
		erreur();
	}
	else if (error == ERR_OK || error == ERR_RECOG_MID_CONF) { // L'ordre a été donn�

		_SysBeep(20);
		if(class==G_comm_commande_robot_AVANCE) { // 110
			set_signal(1, 1, 0);
			clignoter(2, 200);
		}
		else if(class==G_comm_commande_robot_RECULE ) { // 111
			set_signal(1, 1, 1);
			clignoter(3, 200);
		}
		else if(class==G_comm_commande_robot_GAUCHE ) { // 100
			set_signal(1, 0, 0);
			clignoter(4, 200);
		}
		else if(class==G_comm_commande_robot_DROITE ) { // 101
			set_signal(1, 0, 1);
			clignoter(5, 200);
		}
		else if(class==G_comm_commande_robot_STOP ) { // 000
			set_signal(0, 0, 0);
			clignoter(1, 200);
		}
		else { // L'ordre n'est pas reconnu
			erreur();
		}
	}
	else {
		erreur();
	}
}

void loop_bouton () {
	
// On attend que l'utilisateur appuie sur le bouton A 

	_GreenOn();
	if(btnFlag == 0) {
		if (ButtonAPressed) btnFlag = 1;
	}
	else if(!ButtonAPressed) {
		btnFlag = 0;
		ordre();
	}
}

void loop_voice () {

// Attend que l'utilisateur parle pour lancer l'�coute
	
	uchar error;
	
	_GreenOff();
	error = _T2SI((long)&nn_commande_robot, (long)&gsearch_comm_commande_robot, T2SI_DEFAULT_KNOB, T2SI_DEFAULT_TIMEOUT, T2SI_DEFAULT_TRAILING, &results);
	if(error != ERR_RECOG_LOW_CONF) ordre();
	else erreur();
}

void toggle_voice_auto () {
	
// Active/d�sactive le mode d�tection de voix automatique
	
	if(ButtonBPressed && (autoFlag == 0 || autoVoice == 1) ) {
		autoFlag = 1;
		autoVoice = 1 - autoVoice;
 		_SysBeep(300);
		_DelayMilliSecs(1000);
		_SysBeep(300);
	}
	else if(!ButtonBPressed)
		autoFlag = 0;
}

void main(void) {

	// Début du boot : le voyant ROUGE est allumé
	_GreenOff();
	_YellowOff();
	_RedOn();

	adr0 = GPIO(OUTPUT_ADR);
	adr1 = GPIO(OUTPUT_ADR + 1);
	adr2 = GPIO(OUTPUT_ADR + 2);

	_SetOutput(adr0,0); // Les sorties sont mises à zéro par défaut
	_SetOutput(adr1,0);
	_SetOutput(adr2,0);

	// Fin du boot : on met allume le voyant VERT et on éteint les autres
	_SysBeep(300);
	_DelayMilliSecs(1000);
	_SysBeep(300);
	_GreenOff();
	_YellowOff();
	_RedOff();

    while (1) {
		
		if(autoVoice == 0)
			loop_bouton();
		else 
			loop_voice();
		
		// Appuyer sur le bouton C force l'envoi du signal STOP
		
		if(ButtonCPressed) {
			set_signal(0, 0, 0);
			_RedOn();
			_SysBeep(5000);
			_DelayMilliSecs(1000);
			_RedOff();
			_DelayMilliSecs(1000);
		}
		
		// Appuyer sur le bouton D force l'envoi du signal KLAXON, puis renvoie l'ancien ordre donn�
		
		if(ButtonDPressed) {
			
			int o0 = _ReadPin(adr0);
			int o1 = _ReadPin(adr1);
			int o2 = _ReadPin(adr2);
			set_signal(0, 0, 1);
			_YellowOn();
			_RedOn();
			_DelayMilliSecs(10000);
			set_signal(o0, o1, o2);
			_RedOff();
			_YellowOff();
		}
		
		toggle_voice_auto();
    } 
}
