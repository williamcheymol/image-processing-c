// William CHEYMOL / 1A

#include <stdio.h> // for printf and fprintf
#include <stdlib.h> // for alloc funcs
#include <string.h> // for strcmp
#include "pictures.h"
#include "filename.h"
#include "pixel.h"
#include "lut.h"

/* 
 * @requires max <= RAND_MAX
 * @assigns nothing
 * @ensures returns a pseudo-random value between 0 and max included. If rand() is assumed to be uniform, so is this function 
 */

int rand_up_to(int max){
  int r;
  do
    r = rand();
  while (r >= __LONG_MAX__ - (__LONG_MAX__ % (max + 1)));
  return r % (max + 1);
}

/*
 * @requires width > 0, height > 0 , channels = 1 or channels = 3.
 * @assigns result.
 * @ensures creates a picture with the information given.
 */

// Questions principales

picture create_picture(unsigned int width, unsigned int height, unsigned int channels){
    
    // Vérification des données
    if(width<=0){
        fprintf(stderr,"Erreur : la largeur doit être strictement positive.\n");
    }
    if(height<=0){
        fprintf(stderr,"Erreur : la hauteur doit être strictement positive.\n");
    }
    if(channels!=3 && channels!=1){
        fprintf(stderr,"Erreur : le nombre de canaux doit être égal à 1 ou 3.\n");
    }

    // Création de notre image
    picture picres;
    picres.width=width;
    picres.height=height;
    picres.channels=channels;

    // Si l'image est au format PPM
    if (picres.channels==3){
        picres.data = (pixel_ppm*)malloc(width * height * sizeof(pixel_ppm));
        if (!picres.data) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            picres.width=0;
            picres.height=0;
            picres.channels=0;
            free(picres.data);
            picres.data=NULL;
        }
    }
    // Si l'image est au format PGM
    else{
        picres.data = (pixel_pgm*)malloc(width * height * sizeof(pixel_pgm));
        if (!picres.data) { // Vérification que l'allocation ait bien réussi.
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            picres.width=0;
            picres.height=0;
            picres.channels=0;
            free(picres.data);
            picres.data=NULL;
        }
    }
    return picres;
}

/*
 * @requires a valid picture address.
 * @assigns nothing.
 * @ensures erases all the information of the picture.
 */

void clean_picture(picture* p){
    p->width=0;
    p->height=0;
    p->channels=0;
    free(p->data);
    p->data=NULL;
}

/* @requires a valid file address
 * @assigns nothing
 * @ensures permits to skip the commentaries ( lines starting with # ) of a file
 */

void commentaries(FILE* file) {
    long position;
    char line[256];  // Un tableau pour stocker chaque ligne lue
        while (fgets(line, sizeof(line), file)) {
            if ((unsigned char)line[0] == 35 || (unsigned char)line[0] == 10) { // On saute les sauts de lignes ( i.e. les lignes vides ) et les # ( codes ASCII )
                position = ftell(file); // Utile pour sauvegarder la bonne ligne
                continue;  // Passer à la ligne suivante
            } else {
                fseek(file, position, SEEK_SET); // Utile pour revenir à la bonne ligne
                break;  // Si une ligne valide est trouvée, sortir de la boucle
            }
        }
}

/* 
 * @requires a valid path.
 * @assigns result.
 * @ensures ransmits all the data into a freshly built picture.
 */

picture read_picture(const char* path) {
    FILE* file = fopen(path, "rb"); // Permet d'ouvrir le fichier afin de le manipuler
    picture picres;
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier en lecture.\n"); 
        clean_picture(&picres);
        return picres;
    }

    char magic_number[3];
    fscanf(file, "%2s", &magic_number); // Lecture de la première ligne et donc du numéro magique

    if (strcmp(magic_number, "P6") != 0 && strcmp(magic_number, "P5") != 0) { // Permet de vérifier le format de l'image P5 ou P6
        fprintf(stderr, "Format non supporté.\n");
        fclose(file);
        clean_picture(&picres);
        return picres;
    }

    int width, height, max_value;
    commentaries(file); // Premier appel de la procédure commentaries, utile pour sauter les commentaires entre le numéro magique et la taille
    fscanf(file, "%d %d", &width, &height); // Lecture des dimensions de l'image
    commentaries(file); // Deuxième appel de la procédure commentaries, utile pour sauter les commentaires entre la taille et max_value
    fscanf(file, "%d", &max_value); // Lecture de max_value de l'image
    commentaries(file); // Troisième appel de la procédure commentaries, utile pour sauter les commentaires entre max_value et le début de l'image en bytes
    
    if(!width || !height) { // Permet de vérifier les dimensions de l'image
        fprintf(stderr, "Erreur : dimensions invalides.\n");
        fclose(file);
        clean_picture(&picres);
        return picres;
    }

    if(max_value<1 || max_value > 255){ // Permet de vérifier la valeur maximale des pixels
        fprintf(stderr, "Erreur : valeur maximale des pixels invalide : %d\n",max_value);
        fclose(file);
        clean_picture(&picres);
        return picres;
    }

    float correction=255/max_value; // Correction possible si max_value n'est pas égal à 255

    picres.width = width;
    picres.height = height;

    size_t bytes_read; // Utile pour la vérification qui arrive
    int verif; // Compteur permettant de vérifier que le nombre de pixels lus correspond bien aux dimensions déjà trouvées
    

    // Cas des images PPM
    if (strcmp(magic_number, "P6") == 0) { 
        picres.channels = 3;
        verif = picres.width * picres.height * picres.channels;
        size_t data_size=picres.width * picres.height * picres.channels;
        picres.data = (pixel_ppm*)malloc(data_size * sizeof(pixel_ppm));

        if (!picres.data) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            clean_picture(&picres);
            return picres;
        }

        // Lecture par blocs
        size_t bloc_size = 1024;  // Taille du bloc 
        byte *buffer = malloc(bloc_size);
        size_t bytes_lus = 0;
        size_t n;

        // Pointeur pour la copie dans img.data
        // Conversion afin de manipuler l'indicage des tableaux
        pixel_ppm *pixel_ptr = (pixel_ppm*)picres.data;

        size_t pixel_index = 0; // Compteur global pour les pixels dans img.data
        byte value;
        while ((n = fread(buffer, 1, bloc_size, file)) > 0) {
            bytes_lus+=n;
            for (size_t i = 0; i < n; ++i) {
                // Détermine quelle composante du pixel actuel on remplit
                value = buffer[i];
                enum color current_color;
                if (pixel_index % 3 == 0) 
                    current_color = RED;
                else if (pixel_index % 3 == 1) 
                    current_color = GREEN;
                else 
                    current_color = BLUE;
                set_pixel_ppm_component_value(((pixel_ppm*)picres.data) + (pixel_index / 3), current_color, (byte)(value*correction));

                pixel_index++; // Avance dans l'image
            }
        }
        free(buffer);
        if (bytes_lus != verif){
            fprintf(stderr, "Erreur : lecture incohérente avec les dimensions données.\n");
            fclose(file);
            clean_picture(&picres);
            return picres;
        }
    }

    // Cas des images PGM
    else {
        picres.channels = 1;
        verif = picres.width * picres.height * picres.channels;
        picres.data = (pixel_pgm*)malloc(width * height * sizeof(pixel_pgm));
        if (!picres.data) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            clean_picture(&picres);
            return picres;
        }

        // Lecture par blocs
        size_t bloc_size = 1024;  // Taille du bloc 
        byte *buffer = malloc(bloc_size);
        size_t bytes_lus = 0;
        size_t n;

        // Pointeur pour la copie dans img.data
        // Conversion afin de manipuler l'indicage des tableaux
        pixel_pgm *pixel_ptr = (pixel_pgm*)picres.data;

        while ((n = fread(buffer, 1, bloc_size, file)) > 0) {
            bytes_lus += n;
            for (size_t i = 0; i < n / sizeof(pixel_pgm); ++i) {
                byte value=buffer[i];
                set_pixel_pgm_value(pixel_ptr,(byte)(value*correction));

                pixel_ptr++; // Avance dans l'image
            }
        }
        free(buffer);
        if (bytes_lus != verif){
            fprintf(stderr, "Erreur : lecture incohérente avec les dimensions données.\n");
            fclose(file);
            clean_picture(&picres);
            return picres;
        }
    }
    fclose(file);
    return picres;
}

/* 
 * @requires a valid path
 * @assigns result
 * @ensures writes the picture, transmits all the data into a PPM or PGM file, with the filename associated
 */

int write_picture(picture p, const char* filename){
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier en écriture.\n");
        return 0;
    }

    // Copie du filename afin de le manipuler
    char* copy = strdup(filename);
    if (copy == NULL) {
        fprintf(stderr,"Erreur d'allocation mémoire.\n");
        return 0;
    }

    // Cas des images PPM
    if (p.channels==3){ // Vérification du format
        if (strcmp("ppm",ext_from_path(copy))!=0){
            fprintf(stdout,"Erreur : les extensions ne correspondent pas.\n");
            free(copy);
            return 0;
        }

        // Écriture de l'en-têtfe
        fprintf(file, "P6\n%d %d\n%d\n",p.width,p.height,255);

        // Écrire les pixels en binaire
        if (fwrite(p.data, sizeof(pixel_ppm), p.width * p.height, file) != p.width * p.height) {
            perror("Erreur d'écriture dans le fichier.\n");
            free(copy);
            fclose(file);
            return 0;
        }
    }

    // Cas des images PGM
    else if (p.channels==1){ // Vérification du format
        if (strcmp("pgm",ext_from_path(copy))!=0){
            fprintf(stderr,"Erreur : les extensions ne correspondent pas.\n");
            free(copy);
            return 0;
        }
    
        // Écriture de l'en-tête
        fprintf(file, "P5\n%d %d\n%d\n",p.width,p.height,255);

        // Écrire les pixels en binaire
        if (fwrite(p.data, sizeof(pixel_pgm), p.width * p.height, file) != p.width * p.height) {
            perror("Erreur d'écriture dans le fichier.\n");
            free(copy);
            fclose(file);
            return 0;
        }
    }
    else{
        fprintf(stderr,"Format invalide.\n");
        free(copy);
        fclose(file);
        return 0; // Retour d'erreur
    }

    free(copy);
    fclose(file);
    return 1;  
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns a copy of the picture given
 */

picture copy_picture(picture p){
    picture picres = create_picture(p.width,p.height,p.channels);
    size_t data_size = p.width*p.height;
    if(p.channels==3){
        for (size_t i = 0; i < data_size; i++){
            for (enum color c = RED; c <= BLUE; c++){
                set_pixel_ppm_component_value((pixel_ppm*)picres.data+i, c, get_pixel_ppm_component_value((pixel_ppm*)p.data+i, c));
            }
        }
    }
    else{
        for (size_t i = 0; i < data_size; i++) {
            set_pixel_pgm_value((pixel_pgm*)picres.data+i, get_pixel_pgm_value((pixel_pgm*)p.data+i));
        }
    }
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns 1 if the picture is empty, 0 else
 */

int is_empty_picture(picture p){
    if(p.width==0 || p.height==0 || p.channels==0 || p.data==NULL)
        return 1;
    else
        return 0;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns 1 if the picture is in PGM format, 0 else
 */

int is_gray_picture(picture p){
    return(p.channels==1);
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures returns 1 if the picture is in PPM format, 0 else
 */

int is_color_picture(picture p){
    return(p.channels==3);
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures prints width, height, and channels of the given picture
 */

void info_picture(picture p){
    printf("Caractéristiques de l'image : %d x %d x %d\n",p.width,p.height,p.channels);
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures converts the picture from format PGM to format PPM
 */

struct picture convert_to_color_picture(picture p){
    picture picres;
    if (is_color_picture(p)){ // Si elle est déjà colorée, on se contente de renvoyer une copie
        picres=copy_picture(p);
    }
    else{
        picres=create_picture(p.width,p.height,3);
        size_t data_size=p.width*p.height;
        for (size_t i = 0; i < data_size; i++){
            for (enum color c = RED; c <= BLUE; c++){
                set_pixel_ppm_component_value((pixel_ppm*)picres.data+i, c, get_pixel_pgm_value((pixel_pgm*)p.data+i));
            }
        }
    }
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures converts the picture from format PPM to format PGM
 */

picture convert_to_grey_picture(picture p){
    struct picture picres;
    if (is_gray_picture(p)){ // Si elle est déjà en niveaux de gris, on se contente de renvoyer une copie
        picres=copy_picture(p);
    }
    else{
        picres=create_picture(p.width,p.height,1);
        size_t data_size=p.width*p.height;
        for(size_t i=0;i<data_size;i++){ // On utilise la formule donnée
            byte value=get_luminosity((pixel_ppm*)p.data+i);
            set_pixel_pgm_value((pixel_pgm*)picres.data+i,value);
        }
    }
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures splits the picture into a table of three pictures corresponding to levels of red, green and blue in the original picture
 */

picture* split_picture(struct picture p){
    if (is_gray_picture(p)){
        picture* picres = malloc(sizeof(struct picture));
        *picres=copy_picture(p);
        return picres;
    }
    else{
        size_t data_size = p.width * p.height;
        picture* picres = malloc(3*sizeof(struct picture));
        for(int i=0;i<3;i++) // On remplit les données de nos trois images
            picres[i]=create_picture(p.width,p.height,1);
        for(size_t i=0;i<data_size;i++){
            set_pixel_pgm_value((pixel_pgm*)picres[0].data+i,get_pixel_ppm_component_value((pixel_ppm*)p.data+i, RED));
            set_pixel_pgm_value((pixel_pgm*)picres[1].data+i,get_pixel_ppm_component_value((pixel_ppm*)p.data+i, GREEN));
            set_pixel_pgm_value((pixel_pgm*)picres[2].data+i,get_pixel_ppm_component_value((pixel_ppm*)p.data+i, BLUE));
        }
        return picres;
    }
}

/*
 * @requires three pictures in PGM format
 * @assigns nothing
 * @ensures merges the three pictures corresponding to the red, green and blue components into a final picture in the PPM format
 */

picture merge_picture(picture red, picture green, picture blue){
    picture picres;
    // Vérification du format des images
    if(red.channels!=1 || green.channels!=1 || blue.channels!=1){
        fprintf(stderr,"Erreur : il faut que les images soient au format PGM.\n");
        clean_picture(&picres);
        return picres;
    }
    // Vérification de la taille des images
    if(!(red.width==green.width && blue.width==green.width) || !(red.height==green.height && blue.height==green.width)){
        fprintf(stderr,"Erreur : il faut que les images soit de même taille.\n");
        clean_picture(&picres);
        return picres; 
    }

    picres=create_picture(red.width,red.height,3);
    size_t data_size = picres.width * picres.height;
    for(size_t i=0;i<data_size;i++){
        set_pixel_ppm_component_value((pixel_ppm*)picres.data+i, RED, get_pixel_pgm_value((pixel_pgm*)red.data+i));
        set_pixel_ppm_component_value((pixel_ppm*)picres.data+i, GREEN, get_pixel_pgm_value((pixel_pgm*)green.data+i));
        set_pixel_ppm_component_value((pixel_ppm*)picres.data+i, BLUE, get_pixel_pgm_value((pixel_pgm*)blue.data+i));
    }
    return picres;
}   

/*
 * @requires factor>0
 * @assigns nothing.
 * @ensures brightens the picture, with the factor given
 */

picture brighten_picture(picture p, double factor){
    picture picres;
    // Vérification que le facteur est positif
    if (factor<0){
        fprintf(stderr,"Erreur : le facteur doit être positif.\n");
        clean_picture(&picres);
        return picres;
    }
    picres=create_picture(p.width,p.height,p.channels);
    // Cas des images PPM
    if (p.channels==3){
        size_t data_size = picres.width * picres.height;
        for(size_t i=0;i<data_size;i++){
            for (enum color c = RED; c <= BLUE; c++){
                byte int_value=(int)get_pixel_ppm_component_value((pixel_ppm*)p.data+i,c);
                int value = int_value*factor;
                // Vérification du dépassement
                if (value>255)
                    value=255;
                set_pixel_ppm_component_value((pixel_ppm*)picres.data+i,c,(byte)value);
                }
            }
        }
    // Cas des images PGM
    if (p.channels==1){
        size_t data_size = picres.width * picres.height;
        for(size_t i=0;i<data_size;i++){
            byte int_value=(int)get_pixel_pgm_value((pixel_pgm*)p.data+i);
            int value = int_value*factor;
            // Vérification du dépassement
            if (value>255)
                value=255;
            set_pixel_pgm_value((pixel_pgm*)picres.data+i,(byte)value);
        }
    }
    return picres;
}

/*
 * @requires nothing.
 * @assigns nothing.
 * @ensures melts the picture. The effect is slow : it is possible to execute this function more than one time to increase the effect
 */

picture melt_picture(picture p, int n){
    picture picres;
    // Vérification que l'entier n est positif
    if (n<0){
        fprintf(stderr,"Erreur : le nombre de pixels à faire fondre doit être positif.\n");
        clean_picture(&picres);
        return picres;
    }
    picres=copy_picture(p);
    // Cas des images PPM
    if(p.channels==3){
        for(int i=0;i<n;i++){
            int x=rand_up_to(p.width-1); // Abscisse
            int y=rand_up_to(p.height-1); // Ordonnée
            if(y>0){
                byte luminance1=get_luminosity((pixel_ppm*)p.data+p.width*y+x);
                byte luminance2=get_luminosity((pixel_ppm*)p.data+p.width*y+x-p.width);
                if (luminance1>luminance2){ // Si le pixel au - dessus est plus sombre que celui choisi
                    set_pixel_ppm_component_value((pixel_ppm*)picres.data+p.width * y + x,RED,get_pixel_ppm_component_value((pixel_ppm*)p.data+p.width * y + x-p.width,RED));
                    set_pixel_ppm_component_value((pixel_ppm*)picres.data+p.width * y + x,GREEN,get_pixel_ppm_component_value((pixel_ppm*)p.data+p.width * y + x-p.width,GREEN));
                    set_pixel_ppm_component_value((pixel_ppm*)picres.data+p.width * y + x,BLUE,get_pixel_ppm_component_value((pixel_ppm*)p.data+p.width * y + x-p.width,BLUE));
                }
            }
        }
    }
    // Cas des images PGM
    if(p.channels==1){
        for(int i=0;i<n;i++){
            int x=rand_up_to(p.width-1); // Abscisse
            int y=rand_up_to(p.height-1); // Ordonnée
            if(y>0){
                if (get_pixel_pgm_value((pixel_pgm*)p.data+p.width * y + x)>get_pixel_pgm_value((pixel_pgm*)p.data+p.width * y + x-p.width)) // Si le pixel au - dessus est plus sombre que celui choisi
                    set_pixel_pgm_value((pixel_pgm*)picres.data+p.width * y + x,get_pixel_pgm_value((pixel_pgm*)p.data+p.width * y + x-p.width));
            }
        }
    }
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures inverts the picture, using a LUT
 */

picture inverse_picture(picture p){
    picture picres=copy_picture(p);
    // Création de la LUT
    lut inversion=create_lut(256);
    for(int i=0;i<256;i++){
        set_lut_value(inversion,i,255-i);
    }
    apply_lut(&picres,inversion);
    clean_lut(inversion);
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures normalizes a picture, using a LUT
 */

picture normalize_dynamic_picture(picture p){
    picture picres=copy_picture(p);

    // Cas des images PPM
    // La méthode : on divise l'image en trois composantes, on normalise chaque composante et on reforme l'image
    if(p.channels==3) { 
        picture* picintermerdiate=split_picture(p);
        return merge_picture(normalize_dynamic_picture(picintermerdiate[0]),normalize_dynamic_picture(picintermerdiate[1]),normalize_dynamic_picture(picintermerdiate[2]));
    }
    // Cas des images PGM
    if (p.channels==1){
        byte max=0;
        byte min=255;
        size_t data_size=p.width*p.height;
        for(size_t i=0;i<data_size;i++){
            if(get_pixel_pgm_value((pixel_pgm*)p.data+i)>max)
                max=get_pixel_pgm_value((pixel_pgm*)p.data+i);
            if(get_pixel_pgm_value((pixel_pgm*)p.data+i)<min)
                min=get_pixel_pgm_value((pixel_pgm*)p.data+i);
        }
        if(max==min) // Si l'image est totalement uniforme : pas traitement à faire
            return picres;
        lut* normalization = create_lut(256);
        for (int i = 0; i < 256; i++) {
            if (i < min || i > max) {
                set_lut_value(normalization, i, 0); // Les valeurs hors plage sont ignorées
            } else {
                byte normalized_value = (byte)(((i - min) * 255) / (max - min));
                set_lut_value(normalization, i, normalized_value);
            }
        }
        apply_lut(&picres,normalization);
        clean_lut(normalization);
        return picres;
    }
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures limits the levels of a picture by nb_levels, using a LUT For a PPM, works on every component
 */

picture set_levels(picture p, byte nb_levels){
    picture picres=copy_picture(p);
    // Cas des images PPM : on divise l'image en trois composantes, on applique la fonction chaque composante et on reforme l'image
    if (p.channels==3){
        picture* res=split_picture(picres);
        picres=merge_picture(set_levels(res[0],nb_levels),set_levels(res[1],nb_levels),set_levels(res[2],nb_levels));
        return picres;
    }
    // Cas des images PGM
    if (p.channels==1){

        // Création de la LUT
        lut* levels=create_lut(nb_levels);
        int step_size = 256 / nb_levels;
        for (int i = 0; i < nb_levels; i++) {
            byte stair_value = (byte)(i * step_size);
            set_lut_value(levels, i, stair_value);
        }

        // Application de la LUT : on ne peut pas utiliser apply_lut ici étant donné que notre LUT est limitée
        size_t data_size=p.width*p.height;
        for (size_t i = 0; i < data_size; i++) {
            byte original_value = get_pixel_pgm_value((pixel_pgm*)picres.data + i);
            int palier = original_value / step_size; // Calcul du palier
            byte new_value = get_lut_value(levels, palier);
            set_pixel_pgm_value((pixel_pgm*)picres.data + i, new_value);
        }
        return picres;
    }
}

/*
 * @requires nothing.
 * @assigns nothing.
 * @ensures returns the difference ( in absolute value ) between two pictures.
 */

picture diff_picture(picture p1, picture p2){
    picture picres;
    if(p1.channels!=p2.channels){
        fprintf(stderr,"Erreur : les deux images doivent être au même format.\n");
        clean_picture(&picres);
        return picres;
    }
    if(p1.width!=p2.width || p1.height!=p2.height){
        fprintf(stderr,"Erreur : les deux images doivent être de mêmes dimensions.\n");
        clean_picture(&picres);
        return picres;
    }
    picres=create_picture(p1.width,p1.height,p1.channels);

    // Cas des images PPM
    if(p1.channels==3){
        size_t data_size=p1.width*p1.height;
        for(size_t i=0;i<data_size;i++){
            for (enum color c = RED; c <= BLUE; c++){
            byte value=(byte)abs((int)get_pixel_ppm_component_value((pixel_ppm*)p1.data+i,c)-(int)get_pixel_ppm_component_value((pixel_ppm*)p2.data+i,c));
            set_pixel_ppm_component_value((pixel_ppm*)picres.data+i,c,value);
            }
        }
    }
    // Cas des images PGM
    if(p1.channels==1){
        size_t data_size=p1.width*p1.height;    
        for(size_t i=0;i<data_size;i++){
            byte value=(byte)abs((int)get_pixel_pgm_value((pixel_pgm*)p1.data+i)-(int)get_pixel_pgm_value((pixel_pgm*)p2.data+i));
            set_pixel_pgm_value((pixel_pgm*)picres.data+i,value);
        }
    }
    return picres;
}

/*
 * @requires p2 in PGM format
 * @assigns nothing
 * @ensures returns multiplication between two pictures
 */

picture mult_picture(picture p1, picture p2){
    picture picres;
    if(p2.channels!=1){
        fprintf(stderr,"Erreur : le masque doit être au format PGM.\n");
        clean_picture(&picres);
        return picres;
    }
    if(p1.width!=p2.width || p1.height!=p2.height){
        fprintf(stderr,"Erreur : les deux images doivent être de mêmes dimensions.\n");
        clean_picture(&picres);
        return picres;
    }
    picres=create_picture(p1.width,p1.height,p1.channels);

    // Cas des images PPM
    if(p1.channels==3){
        size_t data_size=p1.width*p1.height;
        for(size_t i=0;i<data_size;i++){
            for (enum color c = RED; c <= BLUE; c++){
            byte value=(byte)((int)get_pixel_ppm_component_value((pixel_ppm*)p1.data+i,c)*(int)get_pixel_pgm_value((pixel_pgm*)p2.data+i)/255);
            set_pixel_ppm_component_value((pixel_ppm*)picres.data+i,c,value);
            }
        }
    }

    // Cas des images PGM
    if(p1.channels==1){
        size_t data_size=p1.width*p1.height;
        for(size_t i=0;i<data_size;i++){
            byte value=(byte)((int)get_pixel_pgm_value((pixel_pgm*)p1.data+i)*(int)get_pixel_pgm_value((pixel_pgm*)p2.data+i)/255);
            set_pixel_pgm_value((pixel_pgm*)picres.data+i,value);
        }
    }
    return picres;
}

/*
 * @requires p3 in PGM format
 * @assigns nothing
 * @ensures mixes two PPM or two PGM with a PGM mask
 */

picture mix_picture(picture p1, picture p2, picture p3){
    picture picres;
    if(p1.channels!=p2.channels){
        fprintf(stderr,"Erreur : les deux premières images doivent avoir le même format.\n");
        clean_picture(&picres);
        return picres;
    }
    if(p3.channels!=1){
        fprintf(stderr,"Erreur : le masque qui s'applique doit être au format PGM.\n");
        clean_picture(&picres);
        return picres;
    }
    if(p1.width!=p2.width || p2.width!=p3.width || p1.height!=p2.height || p2.height!=p3.height){
        fprintf(stderr,"Erreur : les images doivent être de mêmes dimensions.\n");
        clean_picture(&picres);
        return picres;
    }
    picres=create_picture(p1.width,p1.height,p1.channels);

    // Cas où les deux images sont au format PPM
    if(p1.channels==3){
        size_t data_size=p1.width*p1.height;
        for(size_t i=0;i<data_size;i++){
            for (enum color c = RED; c <= BLUE; c++){
                float alpha=(float)(get_pixel_pgm_value((pixel_pgm*)p3.data+i)/255);
                byte value=(byte)((1-alpha)*get_pixel_ppm_component_value((pixel_ppm*)p1.data+i,c)+alpha*get_pixel_ppm_component_value((pixel_ppm*)p2.data+i,c));
                set_pixel_ppm_component_value((pixel_ppm*)picres.data+i,c,value);
            }
        }
    }

    // Cas où les deux images sont au format PGM
    if(p1.channels==1){
        size_t data_size=p1.width*p1.height;
        for(size_t i=0;i<data_size;i++){
            float alpha=(float)(get_pixel_pgm_value((pixel_pgm*)p3.data+i)/255);
            byte value=(byte)((1-alpha)*get_pixel_pgm_value((pixel_pgm*)p1.data+i)+alpha*get_pixel_pgm_value((pixel_pgm*)p2.data+i));
            set_pixel_pgm_value((pixel_pgm*)picres.data+i,value);
        }
    }
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures resized the picture with the information given, using the nearest technique
 */

picture resample_picture_nearest(picture p, unsigned int width, unsigned int height){
    picture picres = create_picture(width,height,p.channels);
    // Calcul des rapports : faut - il agrandir l'image ou la rétrécir ?
    float size_ratio_x = (float)p.width / width;
    float size_ratio_y = (float)p.height / height;

    size_t new_data_size=width*height;
    for(size_t i=0;i<new_data_size;i++){
        int x = i%width;
        int y = i/width;
        // Permet de trouver le pixel le plus proche dans l'image originale. Le +0.5 simule un arrondi classique vu que la conversion en int renvoie la partie entière
        int nix = (int)(x*size_ratio_x+0.5); // nix=newindexx
        int niy = (int)(y*size_ratio_y+0.5); // niy=newindexy

        // Cas des images PPM
        if (p.channels == 3) {
            for (enum color c = RED; c <= BLUE; c++) {
                byte pixel_value = get_pixel_ppm_component_value((pixel_ppm*)p.data + niy * p.width + nix, c);
                set_pixel_ppm_component_value((pixel_ppm*)picres.data + y * width + x, c, pixel_value);
            }
        }

        // Cas des images PGM
        else if (p.channels == 1) {
            byte pixel_value = get_pixel_pgm_value((pixel_pgm*)p.data + niy * p.width + nix);
            set_pixel_pgm_value((pixel_pgm*)picres.data + y * width + x, pixel_value);
        }
    }
    return picres;
}

/*
 * @requires nothing
 * @assigns nothing
 * @ensures resized the picture with the information given, using the bilinear interpolation technique
 */

picture resample_picture_bilinear(picture p, unsigned int width, unsigned int height){
    picture picres = create_picture(width,height,p.channels);
    // Calcul des rapports : faut - il agrandir l'image ou la rétrécir ?
    float size_ratio_x = (float)p.width / width;
    float size_ratio_y = (float)p.height / height;

    size_t new_data_size=width*height;
    for(size_t i=0;i<new_data_size;i++){
        int x = i%width;
        int y = i/width;

        float nix = (x*size_ratio_x); // nix=newindexx
        float niy = (y*size_ratio_y); // niy=newindexy
        // Calcul des coordonnées des 4 pixels qui entourent le pixel que l'on traite actuellement
        int x1 = (int)(nix);
        int y1 = (int)(niy);
        int x2 = x1+1;
        int y2 = y1+1;

        // Attention aux bords !
        if (x2>=p.width)
            x2=x1;
        if (y2>=p.height)
            y2=y1;
        
        // Calcul de alpha et de beta
        float alpha = nix - x1;
        float beta = niy - y1;

        // Cas des images PPM
        if(p.channels==3){
            for (enum color c = RED; c <= BLUE; c++) {
                byte px1y1=get_pixel_ppm_component_value((pixel_ppm*)p.data + y1 * p.width + x1,c);
                byte px1y2=get_pixel_ppm_component_value((pixel_ppm*)p.data + y2 * p.width + x1,c);
                byte px2y1=get_pixel_ppm_component_value((pixel_ppm*)p.data + y1 * p.width + x2,c);
                byte px2y2=get_pixel_ppm_component_value((pixel_ppm*)p.data + y2 * p.width + x2,c);
                // On réécrit selon la formule donnée
                byte value=(byte)((1-alpha)*(1-beta)*px1y1+alpha*(1-beta)*px1y2+beta*(1-alpha)*px2y1+alpha*beta*px2y2);
                set_pixel_ppm_component_value((pixel_ppm*)picres.data+y*width+x,c,value);
            }
        }
        // Si l'image est au format PGM
        if(p.channels==1){
            byte px1y1=get_pixel_pgm_value((pixel_pgm*)p.data + y1 * p.width + x1);
            byte px1y2=get_pixel_pgm_value((pixel_pgm*)p.data + y2 * p.width + x1);
            byte px2y1=get_pixel_pgm_value((pixel_pgm*)p.data + y1 * p.width + x2);
            byte px2y2=get_pixel_pgm_value((pixel_pgm*)p.data + y2 * p.width + x2);
            // On réécrit selon la formule donnée
            byte value=(byte)((1-alpha)*(1-beta)*px1y1+alpha*(1-beta)*px1y2+beta*(1-alpha)*px2y1+alpha*beta*px2y2);
            set_pixel_pgm_value((pixel_pgm*)picres.data+y*width+x,value);
        }
    }
    return picres;
}

// Questions bonus

/*
 * @requires factor>0
 * @assigns nothing
 * @ensures brightens the picture, with the factor given, using this time a LUT
 */

picture picture_brighten_lut(picture p, double factor){
    picture picres=copy_picture(p);
    // Vérification que le facteur est positif
    if (factor<0){
        fprintf(stderr,"Erreur : le facteur doit être positif.\n");
        clean_picture(&picres);
        return picres;
    }
    // Création de la LUT
    lut* bright=create_lut(256); // On prend cette taille, car cela évite d'avoir à fonctionner par paliers
    for (int i = 0; i < 256; i++) {
        if(i*factor>255)
            set_lut_value(bright,i,255);
        else
            set_lut_value(bright,i,(int)i*factor);
    }
    apply_lut(&picres,bright);
    return picres;
}
   