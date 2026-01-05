#include <string>
#include <iostream>

// This file is for helper functions that are focused on communicating 
// with the user and would clutter up main 

std::string getUserFile();
std::ostream& userInstructions(std::ostream &os);
bool checkForValidInput(char c);
std::ostream& userEnding(std::ostream &os);