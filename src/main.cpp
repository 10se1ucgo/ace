#include <iostream>

#include "SDL.h"

#include "game_client.h"
#include "scene/menu.h"

int main(int argc, char **argv) {
#ifdef NDEBUG
    try {
#endif
        ace::GameClient client("ACE: \"Ace of Spades\" CliEnt");
        client.set_scene<ace::scene::MainMenuScene>();
        client.run();
        return 0;
#ifdef NDEBUG
    } catch (const std::exception &ex) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", ex.what(), nullptr);
        std::cerr << ex.what() << std::endl;
    } catch (...) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "An unknown error occured.", nullptr);
    }
    return 1;
#endif
}
