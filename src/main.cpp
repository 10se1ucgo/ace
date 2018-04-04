#include "SDL.h"

#include <iostream>

#include "scene/loading.h"
#include "scene/menu.h"

int main(int argc, char **argv) {
    try {
        ace::GameClient client("ACE: \"Ace of Spades\" CliEnt", 800, 600);
        std::string ip(argc > 1 ? argv[1] : "aos://180274501:32887:0.75");
        client.set_scene<ace::scene::LoadingScene>(ip);
        client.run();
        return 0;
    } catch (const std::exception &ex) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", ex.what(), nullptr);
        std::cerr << ex.what() << std::endl;
        throw;
    } catch (...) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "An unknown error occured.", nullptr);
        throw;
    }
    return 1;
}