#include "user_communication.hpp"

// This file is for helper functions that are focused on communicating 
// with the user and would clutter up main

std::string getUserFile() {
  std::cout << "What file would you like to read from?" << std::endl;
  std::string fileName;
  if (!getline(std::cin, fileName)) {
    std::cin.clear();
    std::cin.ignore(256, '\n');
    throw std::runtime_error("An error was encountered while reading from user input.");
  }
  return "../text/"+fileName+".txt";
}

std::ostream& userInstructions(std::ostream &os) {
  os << "In order to play a text based game load a file by typing in a file name." << std::endl <<
  "Then progress by pressing f or 0-9 for dialogue that has options." << std::endl << std::endl <<
  "To create your own text based game, outside of the src directory create a new folder named text." << std::endl
  << "Then name your file anyName.txt." << std::endl 
  << "Inside the file normal lines will be read out one line at a time and always lead to the next file." << std::endl
  << "Lines starting with \\Q are Question lines and is the line right before a dialogue choice." << std::endl
  << "Lines starting with \\O appear after Question lines and are the Options players can choose from." << std::endl 
  << "Lines starting with \\R appear after Option lines and are the Responses a player gets from choosing a dialogue." << std::endl 
  << "\\R lines can either lead to another file by having a \\\" at the beginning and a \" at the end of the line." << std::endl 
  << "\\Q, \\O, and \\R lines should not follow themselves (ex. a \\O line should not have another \\O line after it.)"
  << std::endl << "Files (that have choices) should end with a \\R line. And a \\R line should always follow a \\O line." 
  << std::endl << std::endl;
  return os;
}

bool checkForValidInput(char c) {
    return c == 'f' || std::isdigit(c);
}

std::ostream& userEnding(std::ostream &os) {
  os << "Thank you for playing!" << std::endl;
  return os;
}