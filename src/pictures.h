// William CHEYMOL / 1A

#include "pixel.h" // for byte type
#include <stdio.h> // for FILE type

/*
 * pictures.h
 * Contient des fonctions pour manipuler des images.
 * La fonction read_picture sert à lire une image : elle prend un nom de fichier, lit les informations et les retranscrit dans la structure image que l'on créée.
 * La fonction write_picture fait le processus inverse.
 * Les autres fonctions sont des fonctions de modifications d'images : conversion de PPM en PGM, tests, inversion, normalisation, redimensionnement, ... 
 * Certaines fonctions telles que l'inversion utilisent des LUT ( LOOK UP TABLE ) .
 * D'autres fonctions ont besoin de plusieurs images : multiplication, mix, différence, ...
 */

#ifndef __PICTURES__
#define __PICTURES__

// Type concret picture
typedef struct picture{
    unsigned int width;
    unsigned int height;
    unsigned int channels;
    void *data; // On prendra des pixels ppm ou pgm selon le cas, d'où l'usage de void ici
} picture;

// Questions principales

// Fonction reprise du TP6, permettant de prendre un nombre au hasard entre 0 et max
int rand_up_to(int max);

// Fonction permettant de créer une image à partir des informations données
picture create_picture(unsigned int width, unsigned int height, unsigned int channels);

// Procédure permettant de réinitialiser les informations d'une image
void clean_picture(picture* p);

// Procédure permettant de passer les commentaires, qui peuvent apparaître partout entre la ligne 1 et 4
void commentaries(FILE* file);

// Fonction permettant de lire une image à partir d'un nom de fichier
picture read_picture(const char* path);

// Fonction permettant de réécrire les informations de l'image dans une vraie image portant le nom de fichier donné
int write_picture(picture p, const char* filename);

// Fonction permettant de copier une image
picture copy_picture(picture p);

// Fonction permettant de vérifier si une image est vide ou non
int is_empty_picture(picture p);

// Fonction permettant de vérifier si une image est au format PGM
int is_gray_picture(picture p);

// Fonction permettant de vérifier si une image est au format PPM
int is_color_picture(picture p);

// Fonction permettant d'afficher les détails d'une image (largeur, hauteur, nombre de canaux)
void info_picture(picture p);

// Fonction permettant de convertir une image PGM au format PPM
picture convert_to_color_picture(picture p);

// Fonction permettant de convertir une image PPM au format PGM
picture convert_to_grey_picture(picture p);

// Fonction permettant de séparer les composantes rouge, verte et bleue d'une image
picture* split_picture(picture p);

// Fonction permettant de recomposer une image avec trois images correspondant aux composantes rouge, verte et bleue
picture merge_picture(picture red, picture green, picture blue);

// Fonction permettant d'éclaircir une image d'un certain facteur
picture brighten_picture(picture p, double factor);

// Fonction permettant de faire fondre une image : on choisit number pixels au hasard dans l'image et si le pixel au - dessus est plus sombre, alors le pixel original prend la valeur du pixel au dessus
// Effet peu visible sur une itération : ne pas hésiter à en faire plusieurs
picture melt_picture(picture p, int number);

// Fonction permettant d'inverser une image. Fonctionne grâce à une LUT
picture inverse_picture(picture p);

// Fonction permettant de normaliser une image. Fonctionne grâce à une LUT
picture normalize_dynamic_picture(picture p);

// Fonction permettant de réduire le nombre de niveaux disponibles pour nos images. Fonctionne grâce à une LUT
picture set_levels(picture p, byte nb_levels);

// Fonction faisant la différence en valeur absolue entre deux images
picture diff_picture(picture p1, picture p2);

// Fonction faisant la multiplication entre deux images. En général la troisième image est un masque
picture mult_picture(picture p1, picture p2);

// Fonction permettant de mélanger deux images grâce à une troisième image, étant un masque
picture mix_picture(picture p1, picture p2, picture p3);

// Fonction permettant de redimensionner une image, utilisant la politique du plus proche voisin
picture resample_picture_nearest(picture image, unsigned int width, unsigned int height);

// Fonction permettant de redimensionner une image, utilisant la politique de l'interpolation bilinéaire
picture resample_picture_bilinear(picture image, unsigned int width, unsigned int height);

// Questions bonus

// Fonction permettant d'éclaircir une image d'un certain facteur. Fonctionne grâce à une LUT
picture picture_brighten_lut(picture p, double factor);

#endif