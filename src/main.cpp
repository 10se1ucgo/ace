#include "SDL.h"

#include <iostream>

#include "game_client.h"
#include "scene/loading.h"
#include "scene/menu.h"
#include "scene/block_test.h"

int main(int argc, char **argv) {
#ifdef NDEBUG
    try {
#endif
        ace::GameClient client("ACE: \"Ace of Spades\" CliEnt");
        std::string ip(argc > 1 ? argv[1] : "aos://180274501:32887:0.75");
        client.set_scene<ace::scene::MapEditor>();
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
