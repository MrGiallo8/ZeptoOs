#include "../includes/stout.h"
#include "../includes/time.h"
#include "../includes/keyboard.h"
#include "../includes/stdfuncs.h"

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