#include "dialogue_object.hpp"

// This file is for functions for DialogueObjects

// Reads in from a txt file to create a DialogueObject (kinda like a graph)
std::unique_ptr<DialogueObject> DialogueObject::makeDialogueFromFile(std::ifstream &infile) {
    std::string currentString;

    // Error handling
    if (!getline(infile, currentString)) {
        if (infile.eof()) {
            // We are at the final object
            return nullptr;
        }
        else {
            throw std::runtime_error("An error was encountered while reading from the file.");
        }
    }

    std::unique_ptr<DialogueObject> newDialogue;

    // Delegating ChoiceDialogue (preline starts with \Q) and SimpleDialogue
    if (currentString.length() >= 3 && currentString.substr(0, 3) == "\\Q ") {
        std::string modifiedString = currentString.substr(3, currentString.length() - 4);
        newDialogue =
            std::make_unique<ChoiceDialogue>(modifiedString, 
                DialogueObject::handleChoiceDialogueFromFile(infile));
    }
    else {
        newDialogue =
            std::make_unique<SimpleDialogue>(currentString, 
                DialogueObject::makeDialogueFromFile(infile));
    }

    // Closed here, as opposed to main, due to \R's potentially having a file.
    infile.close();

    return newDialogue;
}

// Creates children for a ChoiceDialogue object, assumes ChoiceDialogue has already been read
// and what is left are Options and Responses
std::vector<std::unique_ptr<DialogueObject>> 
DialogueObject::handleChoiceDialogueFromFile(std::ifstream &infile) {
    std::vector<std::unique_ptr<DialogueObject>> allOptions;

    // We have passed the \Q string and now have a bunch of \Os leading to \Rs and files
    if (infile.eof()) {
        throw std::invalid_argument("Invalid file. File should not end with a Question (\\Q) dialogue.");
    }

    while (!infile.eof()) {
        // Read from file, get a 'O' string, read again for the 'R' string
        std::string currentString;
        std::string nextString;

        if (!getline(infile, currentString)) {
            throw std::runtime_error("An error was encountered while reading from the file.");
        }

        if (currentString.length() < 3 || currentString.substr(0, 3) != "\\O ") {
            throw std::invalid_argument("Invalid file. After a \\Q line there should be an Option (\\O) dialogue.");
        }

        if (!getline(infile, nextString)) {
            throw std::runtime_error("An error was encountered while reading from the file.");
        }

        if (nextString.length() < 3 || nextString.substr(0, 3) != "\\R ") {
            throw std::invalid_argument("Invalid file. After a \\O line there should be an Response (\\R) dialogue.");
        }

        //Stores response dialogue (next dialogue of choice dialogues)
        std::unique_ptr<DialogueObject> responseDialogue;

        //If the layout is '\"..."' that means read from another file. Handles reading from
        //another file or the current line.
        if (nextString.length() > 5 && nextString[3] == '\\' 
        && nextString[4] == '\"' && nextString[nextString.length() - 1] == '\"') {
            // Reading in from new file
            std::string fileName = nextString.substr(5, nextString.length() - 6);
            std::ifstream newFile("../text/" + fileName + ".txt");
            if (!newFile) {
                throw std::invalid_argument("Invalid file. .../text/" 
                    + fileName + ".txt could not be found");
            }
            responseDialogue = DialogueObject::makeDialogueFromFile(newFile);
        }
        else {
            // Reading from next line
            responseDialogue = std::make_unique<SimpleDialogue>(nextString.substr(3, nextString.length() - 3), nullptr);
        }

        // Form choice dialogue, and use move to ensure no memory loss/scary errors
        std::unique_ptr<DialogueObject> choiceDialogue =
            std::make_unique<SimpleDialogue>(currentString.substr(3, currentString.length() - 3), 
            std::move(responseDialogue));

        allOptions.push_back(std::move(choiceDialogue));
    }

    infile.close();

    if (allOptions.size() > 10) {
        throw std::logic_error("Has more than 10 options for a question. Options past the 10th won't be clickable by the user.");
    }
    return allOptions;
}

// Abstract destructor in a different file since this is pure virtual.
DialogueObject::~DialogueObject(){}

//Factory method? Helps generalize + create new abstract version of pointer in goToNextDialogue 
std::unique_ptr<DialogueObject> SimpleDialogue::makeUniquePointerOf() {
    std::unique_ptr<DialogueObject> abstractPtr = 
        std::make_unique<SimpleDialogue>(m_currentDialogue, std::move(m_nextDialogue));
    return abstractPtr;
}   

//Factory method? Helps generalize + create new abstract version of pointer in goToNextDialogue 
std::unique_ptr<DialogueObject> ChoiceDialogue::makeUniquePointerOf() {
    std::unique_ptr<DialogueObject> abstractPtr = 
        std::make_unique<ChoiceDialogue>(m_currentDialogue, std::move(m_options));
    return abstractPtr;
}

// Goes to next dialogue for a dialogue with only one possible response
std::unique_ptr<DialogueObject> SimpleDialogue::goToNextDialogue(int choice) {
    if (choice != 0) {
        throw std::invalid_argument("User should not have input number for a linear dialogue choice.");
    }
    return std::move(m_nextDialogue);
}

// Goes to next dialogue for a dialogue with multiple possible responses based off the user's
// response
std::unique_ptr<DialogueObject> ChoiceDialogue::goToNextDialogue(int choice) {
    //I'm gonna be honest I don't know if makeUniquePointerOf is needed but it made the error go away
    //And like it makes some sense, because I can't actually instantiate a unique_ptr<DialogueObject>
    //since it's purely virtual and it seems like I need to make a new one because vectors?
    std::unique_ptr<DialogueObject> nextDialogue = m_options.at(choice)->makeUniquePointerOf();
    return nextDialogue;
}

// Shows the current dialogue (current line)
std::ostream &DialogueObject::showCurrentDialogue(std::ostream &os) {
    os << m_currentDialogue << std::endl;
    return os;
}

// Shows the current dialogue and dialogue choices
std::ostream &ChoiceDialogue::showCurrentDialogue(std::ostream &os) {
    os << m_currentDialogue << std::endl;
    for (size_t i = 0; i < m_options.size(); i++) {
        os << i << ". " << m_options[i]->getCurrentDialogue() << std::endl;
    }
    return os;
}
