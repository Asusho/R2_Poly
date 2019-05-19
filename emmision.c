#include <htc.h>
#include <pic.h>

char interval = 96;
char front_to_send = 0; // Le nombre de front à envoyer, s'il est modifié

// Délai d'attente de n * 50 itérations de boucle
void delai (char n) {
	while (n > 0) {
		char i = 0;
		while (i < 50) i++;
		n--;
	}
}

// Envoie un certain nombre de fronts dans l'émetteur
void front (char n) {

// Envoie n fronts
	char i = 0;
	while (i < n) {
		RA2 = 1;
		delai(1);
		RA2 = 0;
		delai(1);
		i++;
	}

// Attend un certain temps entre les trames
	delai(interval);
}

//Main
void main(void){
	PORTA = 0;
    PORTB = 0;
    TRISB = 0b11111111; // Port B en entrée
    TRISA = 0b00000000; // Port A en sortie

	while(1){

		char f = front_to_send;
		if (RB4 == 1 && RB5 == 1 && RB6 == 0) //AVANCE
			f = 8;
		else if (RB4 == 1 && RB5 == 1 && RB6 == 1) //RECULE
			f = 16;
		else if (RB4 == 1 && RB5 == 0 && RB6 == 0) //GAUCHE
			f = 24;
		else if (RB4 == 1 && RB5 == 0 && RB6 == 1) //DROITE
			f = 32;
		else if (RB4 == 0 && RB5 == 0 && RB6 == 0) //STOP
			f = 40;
		else if (RB4 == 0 && RB5 == 0 && RB6 == 1) //KLAXON
			f = 48;

		front(f);
		front_to_send = f;
 	}
}