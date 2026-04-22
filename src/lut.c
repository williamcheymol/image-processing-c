// William CHEYMOL / 1A

#include "lut.h"
#include "pictures.h"
#include <stdlib.h> // for allocs funcs 
#include <stdio.h> // for fprintf

// Déclaration concrète d'une LUT
struct lut {
    unsigned int taille;
    byte* table;
};

/*
 * @requires n>0
 * @assigns result
 * @ensures creates a LUT of size n
 */

lut create_lut(int n) {
    lut l = malloc(sizeof(struct lut));
    
    if (!l){
        fprintf(stderr,"Erreur d'allocation mémoire.\n");
        free(l);
        return l;
    }

    struct lut* lut_ptr = (struct lut*) l;
    lut_ptr->taille = n;
    lut_ptr->table = malloc(n * sizeof(byte));
    if (!lut_ptr->table) {
        fprintf(stderr,"Erreur d'allocation mémoire.\n");
        free(l);
        return NULL;
    }

    // Initialisation de la LUT avec des valeurs par défaut
    for (int i = 0; i < n; i++) {
        lut_ptr->table[i] = 0;
    }
    return l;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures erases all data of the LUT
 */

void clean_lut(lut l) {
    struct lut* lut_ptr = (struct lut*) l;
    free(lut_ptr->table);
    free(lut_ptr);
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns all data ( the table ) of the LUT
 */

void* get_lut_data(lut l) {
    struct lut* lut_ptr = (struct lut*) l;
    return lut_ptr->table;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns the size of the LUT
 */

unsigned int get_lut_size(lut l) {
    struct lut* lut_ptr = (struct lut*) l;
    return lut_ptr->taille;
}

/*
 * @requires a valid index
 * @assigns nothing
 * @ensures returns the value of l[index]
 */

byte get_lut_value(lut l, int index){
    struct lut* lut_ptr = (struct lut*) l;
    return lut_ptr->table[index];
}

/*
 * @requires a valid index
 * @assigns nothing
 * @ensures sets the value of l[index]
 */

void set_lut_value(lut l, int index, byte value) {
    struct lut* lut_ptr = (struct lut*) l;
    if (index >= 0 && index < lut_ptr->taille) {
        lut_ptr->table[index] = value;
    }
}

/*
 * @requires a valid picture address
 * @assigns nothing
 * @ensures apply a LUT on a picture
 */

void apply_lut(picture* p, lut l) {

    byte* lut_data = (byte*) get_lut_data(l);
    if (!lut_data) {
        fprintf(stderr, "Erreur : LUT invalide.\n");
        exit(EXIT_FAILURE);
    }

    // Cas des images PPM
    if (p->channels == 3) {
        size_t data_size = p->width * p->height;
        for (size_t i = 0; i < data_size; i++) {
            for (enum color c = RED; c <= BLUE; c++) {
                byte original_value = get_pixel_ppm_component_value((pixel_ppm*) p->data + i, c);
                byte new_value = lut_data[original_value];
                set_pixel_ppm_component_value((pixel_ppm*) p->data + i, c, new_value);
            }
        }
    }

    // Cas des images PGM
    else if (p->channels == 1) {
        size_t data_size = p->width * p->height;
        for (size_t i = 0; i < data_size; i++) {
            byte original_value = get_pixel_pgm_value((pixel_pgm*) p->data + i);
            byte new_value = lut_data[original_value];
            set_pixel_pgm_value((pixel_pgm*) p->data + i, new_value); 
        }
    }
}
