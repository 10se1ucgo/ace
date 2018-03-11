#include "SDL.h"

#include <iostream>
#include <chrono>

#include "common.h"
#include "scene/game.h"
#include "scene/loading.h"


std::default_random_engine eng(std::chrono::system_clock::now().time_since_epoch().count());

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
    } catch (...) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", "An unknown error occured.", nullptr);
    }
    return 1;
}