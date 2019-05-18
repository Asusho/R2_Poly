#include <htc.h> // compilateur hitech C
#include <pic.h>
 
char cpt = 0;
char n = 0;
char i;

char ecart = 4;

void interpreter () {

	//AVANCER
	if(cpt >= 8 - ecart && cpt < 8 + ecart) {
		RB1 = 0;
		RB2 = 0;
		RB3 = 1;
	}
	//RECULER
	else if(cpt >= 16 - ecart && cpt < 16 + ecart) {
		RB1 = 0;
		RB2 = 1;
		RB3 = 1;
	}
	//GAUCHE
	else if(cpt >= 24 - ecart && cpt < 24 + ecart) {
		RB1 = 0;
		RB2 = 1;
		RB3 = 0;
	}
	//DROITE
	else if(cpt >= 32 - ecart && cpt < 32 + ecart) {
		RB1 = 1;
		RB2 = 1;
		RB3 = 0;
	}
	//STOP
	else if(cpt >= 40 - ecart && cpt < 40 + ecart) {
		RB1 = 1;
		RB2 = 1;
		RB3 = 1;
	}
	//KLAXON
	else if(cpt >= 48 - ecart && cpt < 48 + ecart) {
		RB1 = 1;
		RB2 = 0;
		RB3 = 1;
	}
	//ORDRE NON RECONNUs
	else{
		RB1 = 0;
		RB2 = 0;
		RB3 = 0;
	}
}

int main (void){

	PORTA = 0x00;
	PORTB = 0x00;
	TRISA = 0x11111111; // A en entree
	TRISB = 0x10000001;	// B en sortie, RB0 en entree
	
	// Autorise les interruptions
	INTCON = 0x00000000;
	GIE = 1;
	INTE = 1;

	while (1) {
		RB4 = 1;
		while (n > 0) {
			i = 0;
			while (i < 50) i++;
			n--;

			if(n == 0) {
				INTCON = 0x00000000;

				interpreter();
				cpt = 0;

				GIE = 1;
				INTE = 1;
			}
		}
	}
	return 0;
}

// Interruption sur RB0
void interrupt isr(void){

	INTCON = 0x00000000;

	cpt++;
	if(n == 0) n = 80;
	
	INTCON = 0x00000000;
	GIE = 1;
	INTE = 1;
}
