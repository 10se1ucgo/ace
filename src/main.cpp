#include "SDL.h"

#include <iostream>

#include "scene/game.h"
#include "scene/loading.h"
#include "common.h"


std::default_random_engine eng(std::chrono::system_clock::now().time_since_epoch().count());

int main(int argc, char **argv) {
    try {
        ace::GameClient client("ACE: \"Ace of Spades\" CliEnt", 800, 600);
        client.set_scene<ace::scene::LoadingScene>("aos://180274501:40887:0.75");
 //       client.set_scene<ace::scene::LoadingScene>("69.197.190.10", 31887);
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