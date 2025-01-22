#include <Adafruit_GFX.h>    // Inclure la bibliothèque Adafruit GFX
#include <Adafruit_ST7789.h> // Inclure la bibliothèque Adafruit ST7789

// Définition des broches
#define TFT_CS    10 // Chip Select
#define TFT_RST   9  // Réinitialisation de l'écran (peut ne pas être utilisée)
#define TFT_DC    8  // Commande/Détection

// Initialisation de l'écran
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  // Initialisation de l'écran
  tft.init(135, 240); // Initialisation de l'écran avec la résolution spécifiée
  tft.setRotation(1); // Rotation de l'écran si nécessaire (0, 1, 2 ou 3)

  // Effacement de l'écran
  tft.fillScreen(ST77XX_BLACK);

  // Affichage d'un texte
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println("idehack.com");
}

void loop() {
  // Rien à faire dans la boucle loop() dans cet exemple
}