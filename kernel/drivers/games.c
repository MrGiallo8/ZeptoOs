#include "../includes/stout.h"
#include "../includes/time.h"
#include "../includes/keyboard.h"
#include "../includes/stdfuncs.h"

void game_menu(){
    clear_screen();
    bordo();

    write_center("ZEPTO GAMES!",3, BLU_CHIARO);write("\n");
    write_center("0. Exit",6, BLU_CHIARO);
    write_center("1. NUMBER_GUESSER",7, BLU_CHIARO);

    char input[256];
    while (1){
        write("%g%k Games> ",3,12,BIANCO);
        kb_readline(input, 256, pputc);
        write("\n");
        int int_input = str_to_int(input);
        if(int_input == 1){ clear_screen(); number_guesser();return;}
        else if(int_input == 0){return;}
        else { write("%g%k No game found... ", 3, 12, ROSSO);}
    }
}

void number_guesser(){
	for(int i=0;i<80;i++){
		write("%g%b ",i,0,GRIGIO);
	}

    write_center("[ Number Guesser ]",0,NERO);
    char input[256];
    write("%b%k\n\nWelcome to number guesser!\n",NERO,BIANCO);
    write("I will think of a number and you will have to guess it (1-99)!\n");
    uint64_t random_number = random_dig(2);
    write("Number generated !\n");

    int tentativi=0;
    while (1){
        write("Insert the number:");
        kb_readline(input, 256, pputc);
        tentativi++;
        int int_input = str_to_int(input);
        if(int_input > random_number ){
            write("%kLower than ",BIANCO); write(input);write("\n");
        }else if (int_input < random_number){
            write("%kHigher than ",BIANCO); write(input);write("\n");
        }else if(int_input == random_number){
            write("%kCongratulations! You won!",BIANCO);write("\n");
            write("%kAttempts = %d! ",BIANCO,tentativi);
            delay_s(3);
            return;
        }
    }
}
