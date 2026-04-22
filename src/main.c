// William CHEYMOL / 1A

#include <stdio.h> // for fprintf
#include <stdlib.h> // for srand
#include <time.h> // for random funcs
#include <string.h> // for strdup
#include "pictures.h"
#include "filename.h"

// N'hésitez pas à voir le READ_ME_PROJET_WC pour savoir comment l'exécutable marche.

int main(int argc, char* argv[]) {

    // Modification de la graine pour la modélisation du hasard.
    srand(time(NULL));

    // Vérification des arguments
    if(argc != 4) {
        fprintf(stderr,"Erreur : nombre d'arguments entré erroné. Je vous conseille de lire le READ_ME_PROJET_WC.\n");
        return -1;
    } 

    else {
        
        // Copie des filenames afin de les manipuler

        // Filename 1
        const char* input_file1 = argv[1];
        char* copy1 = strdup(input_file1);
        if (copy1 == NULL) {
            fprintf(stderr,"Erreur d'allocation mémoire.\n");
            return -1;
        }
        // Lire l'image depuis le fichier d'entrée
        char* ext1=ext_from_path(copy1);
        char* name1=name_from_path(copy1);
        char* dir1=dir_from_path(copy1);
        picture p1=read_picture(input_file1);

        // Filename 2
        const char* input_file2 = argv[2];
        char* copy2 = strdup(input_file2);
        if (copy2 == NULL) {
            fprintf(stderr,"Erreur d'allocation mémoire.\n");
            return -1;
        }
        // Lire l'image depuis le fichier d'entrée
        char* ext2=ext_from_path(copy2);
        char* name2=name_from_path(copy2);
        char* dir2=dir_from_path(copy2);
        picture p2=read_picture(input_file2);

        // Filename 3
        const char* input_file3 = argv[3];
        char* copy3 = strdup(input_file3);
        if (copy3 == NULL) {
            fprintf(stderr,"Erreur d'allocation mémoire.\n");
            return -1;
        }
        // Lire l'image depuis le fichier d'entrée
        char* ext3=ext_from_path(copy3);
        char* name3=name_from_path(copy3);
        char* dir3=dir_from_path(copy3);
        picture p3=read_picture(input_file3);

        // Est - ce que les images sont dans le même dossier ? Ce serait préférable
        
        if(strcmp(dir1,dir2)!=0 || strcmp(dir2,dir3)!=0){
            fprintf(stderr,"Erreur : les trois fichiers doivent être dans le même dossier. Je vous conseille de lire le READ_ME_PROJET_WC.\n");
            return -1;
        }

        // Vérification du contenu des images
        if(is_empty_picture(p1) || is_empty_picture(p2) || is_empty_picture(p3)){
            fprintf(stderr,"Erreur : une des images est vide. Je vous conseille de lire le READ_ME_PROJET_WC.\n");
            return -1;
        }

        // Vérification du format des images
        if(!is_color_picture(p1) || !is_gray_picture(p2) || !is_gray_picture(p3)){
            fprintf(stderr,"Erreur : les images ne sont pas toutes au bon format. Je vous conseille de lire le READ_ME_PROJET_WC.\n");
            return -1;
        }

        // Question 1
        const char* q1=concat_parts(dir2,name2,"convert_color",ext1);
        write_picture(convert_to_color_picture(p2),q1);

        // Question 2
        const char* q2=concat_parts(dir1,name1,"convert_gray",ext2);
        write_picture(convert_to_grey_picture(p1),q2);

        // Question 3
        const char* q3r=concat_parts(dir1,name1,"red",ext2);
        const char* q3g=concat_parts(dir1,name1,"green",ext2);
        const char* q3b=concat_parts(dir1,name1,"blue",ext2);
        picture* split=split_picture(p1);
        write_picture(split[0],q3r);
        write_picture(split[1],q3g);
        write_picture(split[2],q3b);

        // Question 4
        const char* q4colors=concat_parts(dir1,name1,"brighten",ext1);
        const char* q4gray=concat_parts(dir2,name2,"brighten",ext2);
        write_picture(brighten_picture(p1,1.5),q4colors);
        write_picture(brighten_picture(p2,1.5),q4gray);

        // Question 5
        const char* q5colors=concat_parts(dir1,name1,"melted",ext1);
        const char* q5gray=concat_parts(dir2,name2,"melted",ext2);
        write_picture(melt_picture(p1,p1.width*p1.height*p1.channels*5),q5colors);
        write_picture(melt_picture(p2,p2.width*p2.height*p2.channels*5),q5gray);

        // Question 6
        const char* q6colors=concat_parts(dir1,name1,"inverse",ext1);
        const char* q6gray=concat_parts(dir2,name2,"inverse",ext2);
        picture color_inverted=inverse_picture(p1);
        picture gray_inverted=inverse_picture(p2);
        write_picture(color_inverted,q6colors);
        write_picture(gray_inverted,q6gray);

        // Question 7
        const char* q7=concat_parts(dir2,name2,"dynamic",ext2);
        write_picture(normalize_dynamic_picture(p2),q7);

        // Question 8 : la fonction split s'occupe de faire ce qui est demandé.
        const char* q8=concat_parts(dir1,name1,"dynamic",ext1);
        write_picture(normalize_dynamic_picture(p1),q8);

        // Question 9
        const char* q9colors=concat_parts(dir1,name1,"levels",ext1);
        const char* q9gray=concat_parts(dir2,name2,"levels",ext2);
        write_picture(set_levels(p1,8),q9colors);
        write_picture(set_levels(p2,5),q9gray);

        // Question 10
        const char* q10colors=concat_parts(dir1,name1,"smaller_nearest",ext1);
        const char* q10gray=concat_parts(dir2,name2,"smaller_nearest",ext2);
        write_picture(resample_picture_nearest(p1,(int)p1.width/1.36,(int)p1.height/1.36),q10colors);
        write_picture(resample_picture_nearest(p2,(int)p2.width/1.36,(int)p2.height/1.36),q10gray);

        // Question 11
        const char* q11colors=concat_parts(dir1,name1,"smaller_bilinear",ext1);
        const char* q11gray=concat_parts(dir2,name2,"smaller_bilinear",ext2);
        write_picture(resample_picture_bilinear(p1,(int)p1.width/1.36,(int)p1.height/1.36),q11colors);
        write_picture(resample_picture_bilinear(p2,(int)p2.width/1.36,(int)p2.height/1.36),q11gray);

        // Question 12
        const char* q12colors=concat_parts(dir1,name1,"larger_nearest",ext1);
        const char* q12gray=concat_parts(dir2,name2,"larger_nearest",ext2);
        picture plcn=resample_picture_nearest(p1,(int)p1.width*1.36,(int)p1.height*1.36);
        picture plgn=resample_picture_nearest(p2,(int)p2.width*1.36,(int)p2.height*1.36);
        write_picture(plcn,q12colors);
        write_picture(plgn,q12gray);

        // Question 13
        const char* q13colors=concat_parts(dir1,name1,"larger_bilinear",ext1);
        const char* q13gray=concat_parts(dir2,name2,"larger_bilinear",ext2);
        picture plcb=resample_picture_bilinear(p1,(int)p1.width*1.36,(int)p1.height*1.36);
        picture plgb=resample_picture_bilinear(p2,(int)p2.width*1.36,(int)p2.height*1.36);
        write_picture(plcb,q13colors);
        write_picture(plgb,q13gray);

        // Question 14
        const char* q14colors=concat_parts(dir1,name1,"difference",ext1);
        const char* q14gray=concat_parts(dir2,name2,"difference",ext2);
        write_picture(normalize_dynamic_picture(diff_picture(plcn,plcb)),q14colors);
        write_picture(normalize_dynamic_picture(diff_picture(plgn,plgb)),q14gray);

        // Question 15
        const char* q15colors=concat_parts(dir1,name1,"product",ext1);
        const char* q15gray=concat_parts(dir2,name2,"product",ext2);
        picture multc=mult_picture(p1,p3);
        picture multg=mult_picture(p2,p3);
        write_picture(multc,q15colors);
        write_picture(multg,q15gray);

        // Question 16
        const char* q16colors=concat_parts(dir1,name1,"mixture",ext1);
        const char* q16gray=concat_parts(dir2,name2,"mixture",ext2);
        write_picture(mix_picture(color_inverted,p1,p3),q16colors);
        write_picture(mix_picture(gray_inverted,p2,p3),q16gray);

        return 0;
    }
}