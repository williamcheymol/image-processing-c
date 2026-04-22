// William CHEYMOL / 1A

#include "pixel.h"

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns the value of a component c of a PPM pixel
 */

byte get_pixel_ppm_component_value(pixel_ppm* p, enum color c) {
    return p->components[c];
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures sets the value of a component c of a PPM pixel
 */

void set_pixel_ppm_component_value(pixel_ppm* p, enum color c, byte value) {
    p->components[c] = value;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns the value PGM pixel
 */

byte get_pixel_pgm_value(pixel_pgm* p) {
    return p->intensity;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures sets the value PGM pixel
 */

void set_pixel_pgm_value(pixel_pgm* p, byte value) {
    p->intensity = value;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures gets the luminosity of a PPM pixel
 */

byte get_luminosity(pixel_ppm* p){
    byte luminosity=0.299*get_pixel_ppm_component_value(p,RED)+0.587*get_pixel_ppm_component_value(p,GREEN)+0.114*get_pixel_ppm_component_value(p,BLUE);
    return luminosity;
    }