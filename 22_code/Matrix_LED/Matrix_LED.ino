#include "LedControl.h" // Added for Matrix Led based on MAX7219

LedControl lc=LedControl(51,52,53,1); // For A Mega DIN=51 , CLK=52, CS=53, 1 Matrix Led

void setup() {

  // Initialisation de la matrice LED
  lc.shutdown(0, false);       // shutdown     : "true" active le mode "sommeil", tandis que "false" active le mode "normal"
  lc.setIntensity(0, 5);       // setIntensity : valeur pouvant aller de 0 à 15 (pour 16 niveaux de luminosité, donc)
  lc.clearDisplay(0);          // clearDisplay : éteint toutes les LEDs de la matrice
  
}

void loop() {

  for(int ligne = 0; ligne < 8; ligne++) {                              // Parcours des 8 lignes de la matrice LED
    for(int colonne = 0; colonne < 8; colonne++) {                          // Parcours des 8 colonnes de la matrice LED
      lc.setLed(0, ligne, colonne, true);              // Allumage de la LED située à la position ligne/colonne
      delay(500);                                                  // Maintien de cette LED allumée, un certain temps
      lc.setLed(0, ligne, colonne, false);             // Et extinction de cette LED, avant passage à la led suivante
    }
  }
}