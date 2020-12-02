#include <r-tech1/main.h>

#ifdef USE_ALLEGRO
#include <allegro.h>

int main(int argc, char ** argv){
    return rtech_main(argc, argv);
}
END_OF_MAIN()
#endif

#ifdef USE_ALLEGRO5
#include <allegro5/allegro.h>
int main(int argc, char ** argv){
    return rtech_main(argc, argv);
}
#endif