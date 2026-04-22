// William CHEYMOL / 1A

#include "pixel.h"
#include "pictures.h"

/*
 * lut.h
 * Contient des fonctions pour manipuler des LUT ( LOOK UP TABLE ) : création, nettoyage, accès aux données
 * La dernière procédure sert en particulier à appliquer une LUT à une image
 */

#ifndef __LUT_H__
#define __LUT_H__

// Type abstrait LUT
typedef void* lut;

// Fonction permettant de créer une LUT
lut create_lut(int n);

// Procédure permettant de réinitialiser la taille et de supprimer les données de la LUT
void clean_lut(lut l);

// Procédure permettant d'accéder aux données de la LUT
void* get_lut_data(lut l);

// Fonction permettant d'obtenir la taille de la LUT
unsigned int get_lut_size(lut l);

// Fonction permettant d'obtenir la valeur de l[index]
byte get_lut_value(lut l, int index);

// Procédure permettant de changer la valeur de l[index]
void set_lut_value(lut l, int index, byte value);

// Procédure permettant d'appliquer une LUT à une image
void apply_lut(picture* p, lut l); 

#endif
