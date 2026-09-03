#include "../includes/stout.h"
#include "../includes/time.h"
#include "../includes/keyboard.h"
#include "../includes/stdfuncs.h"

void game_menu(){
    clear_screen(); 
    bordo();write("\n");
    write_center("ZEPTO GAMES!",3, BLU_CHIARO);write("\n");

    write_center("1. NUMBER_GUESSER",6, BLU_CHIARO);

    char input[256];
    while (1){
        write("%g%k Games> ",3,12,BIANCO);
        kb_readline(input, 256, pputc);
        write("\n");
        int int_input = str_to_int(input);
        if(int_input == 1){ clear_screen(); number_guesser();}
        else { write("%kNo game found...",ROSSO);}
    }
}

void number_guesser(){
    char input[256];
    write("\nBenvenuto a numeber guesser!\n");
    write("Pensero ad un numero e tu dovrai indovinarlo (1-99)!\n");
    uint64_t random_number = random_dig(2);
    write("Numero pensato !\n");

    while (1){
        write("Inserisci il numero:");
        kb_readline(input, 256, pputc);
        int int_input = str_to_int(input);
        if(int_input > random_number ){
            write("%kPiu basso di ",BIANCO); write(input);write("\n");
        }else if (int_input < random_number){
            write("%kPiu alto di ",BIANCO); write(input);write("\n");
        }else if(int_input == random_number){
            write("%kComplimenti! Hai vinto!",BIANCO);write("\n");
            return;
        }
    }
}