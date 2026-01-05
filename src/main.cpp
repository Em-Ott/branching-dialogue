// code derived from Will Usher's SDL 2.0 Tutorial
// https://www.willusher.io/pages/sdl2/

#include <iostream>
#include <string>
#include <memory>
#include "dialogue_object.hpp"
#include "user_communication.hpp"

/* SDL
#include <SDL.h>
#include "res_path.hpp"
*/

int main(int argc, char **argv){
  /* SDL
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
    std::cerr << "SDL_Init error: " << SDL_GetError() << std::endl;
    return 1;
  }
  std::cout << "Resource path is: " << getResourcePath() << std::endl;

  SDL_Quit();
  */
  
  //this should be in a separate helper tbh
  std::string userFile;
  try {
    userFile = getUserFile();
  } catch (const std::runtime_error& e) {
    std::cout << "Runtime error was caused by: " << e.what() << std::endl;
    return -1;
  }
  

  std::ifstream infile(userFile);
  while (!infile) {
    //Couldn't open 
    infile.clear();
    infile.close();
    std::cout << "The file could not be opened." << std::endl;
    std::cout << "Try again." << std::endl;
    try {
      userFile = getUserFile();
    } catch (const std::runtime_error& e) {
      std::cout << "Runtime error was caused by: " << e.what() << std::endl;
      return -1;
    }
    infile = std::ifstream(userFile);
  }
   
  

  std::unique_ptr<DialogueObject> current;

  try {
    current = DialogueObject::makeDialogueFromFile(infile);
  } catch (const std::runtime_error& e) {
    std::cout << "Runtime error caused due to: " << e.what() << std::endl;
    return -1;
  } catch (const std::invalid_argument& e) {
    std::cout << "Invalid argument error caused due to: " << e.what() << std::endl;
    return -1;
  } catch (const std::logic_error& e) {
    std::cout << "Logic error caused due to: " << e.what() << std::endl;
    return -1;
  } 

  userInstructions(std::cout);

  while (current != nullptr) {
    char shouldPlayDialogue = ' ';

    current->showCurrentDialogue(std::cout);

    std::cin >> shouldPlayDialogue;

    while (!checkForValidInput(shouldPlayDialogue)) {
      std::cin.ignore(256, '\n');
      std::cin.clear();
      //Should probably add an invalid input cout
      std::cin >> shouldPlayDialogue;
    }

    std::cin.ignore(256, '\n');
    std::cin.clear();

    if (std::isdigit(shouldPlayDialogue)) {
      int numVers = shouldPlayDialogue - '0';
      try {
        current = current->goToNextDialogue(numVers);
      } catch (const std::invalid_argument& e) {
        std::cout << "An invalid argument exception caused due to: " << e.what() << std::endl;
        std::cout << "Please enter f to proceed through the dialogue." << std::endl;
        if (std::string(e.what()) != "User should not have input number for a linear dialogue choice.") {
          return -1;
        }
      } catch (const std::out_of_range& e) {
        std::cout << "An out of range exception has been caused due to: " << e.what() << std::endl;
        std::cout << "Choose a number between 0-9 which is in range of the options given." << std::endl;
      } 
    } else {
      current = current->goToNextDialogue();
    }
  }
  
  userEnding(std::cout);

  return 0;
}
