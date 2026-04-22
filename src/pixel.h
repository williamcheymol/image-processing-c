// William CHEYMOL / 1A

/*
 * pixel.h
 * Contient des fonctions pour manipuler les pixels.
 * On définit deux types différents : les pixels_pgm et les pixels_ppm.
 * Globalement, la seule chose qui change est que là où les pixels_pgm n'ont qu'une seule composante, les pixels_ppm en ont trois.
 */

#ifndef __PIXEL__
#define __PIXEL__

// Type concret byte
typedef unsigned char byte; // On prend ici unsigned char mais on aurait pu prendre unsigned int. Merci de ne pas changer maintenant que les fonctions ont été codées de cette façon.
#define MAX_BYTE 255 ;

// Déclaration de l'énumération des composantes de couleur
enum color {RED, GREEN, BLUE};

// Définition de la structure pixel_ppm
typedef struct pixel_ppm {
    byte components[3];  // Trois composantes pour chaque intensité de couleur
} pixel_ppm;

// Définition de la structure pixel_pgm
typedef struct pixel_pgm{
    byte intensity;  // Un seul octet pour l'intensité du pixel
} pixel_pgm;


// Fonction pour obtenir la composante de couleur c d'un pixel_ppm 
byte get_pixel_ppm_component_value(pixel_ppm* p, enum color c);

// Procédure pour définir la composante de couleur c d'un pixel ppm
void set_pixel_ppm_component_value(pixel_ppm* p, enum color c, byte value);

// Fonction pour obtenir la composante d'un pixel_pgm
byte get_pixel_pgm_value(pixel_pgm* p);

// Procédure pour définir la composante d'un pixel pgm
void set_pixel_pgm_value(pixel_pgm* p, byte value);

// Fonction pour obtenir la luminosité d'un pixel PPM
byte get_luminosity(pixel_ppm* p);

#endif
