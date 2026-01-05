#include <string>
#include <ostream>
#include <fstream>
#include <memory>
#include <vector>
#include <iostream> //for testing 

// This file is for the setup for DialogueObjects 

//This is a dialogue object. It can have simple dialogue or dialogue which leads to many dialogues.
class DialogueObject {
    public:
        DialogueObject(std::string currentDialogue){
            m_currentDialogue = currentDialogue;
        };
        static std::unique_ptr<DialogueObject> makeDialogueFromFile(std::ifstream& infile);
        static std::vector<std::unique_ptr<DialogueObject>> handleChoiceDialogueFromFile(std::ifstream& infile);
        virtual ~DialogueObject() = 0;
        virtual std::ostream& showCurrentDialogue(std::ostream &os);
        virtual std::unique_ptr<DialogueObject> goToNextDialogue(int choice = 0) = 0;
        virtual std::unique_ptr<DialogueObject> makeUniquePointerOf() = 0;
        std::string getCurrentDialogue(){return m_currentDialogue;};
    protected:
        std::string m_currentDialogue;
};

//This is a simple dialogue. A dialogue which can only lead to one dialogue.
class SimpleDialogue : public DialogueObject {
    public:
        SimpleDialogue(std::string currentDialogue, std::unique_ptr<DialogueObject> nextDialogue) 
        : DialogueObject(currentDialogue) {
            m_nextDialogue = std::move(nextDialogue);
        };
        ~SimpleDialogue() {};
        std::unique_ptr<DialogueObject> goToNextDialogue(int choice = 0);
        std::unique_ptr<DialogueObject> makeUniquePointerOf();
    protected:
        std::unique_ptr<DialogueObject> m_nextDialogue;
};

//A dialogue which can lead to multiple dialogues. 
class ChoiceDialogue : public DialogueObject {
    public:
    //since we're taking in a char you can't technically have more than 10 options (0-9) so i guess this could've been an array
        ChoiceDialogue(std::string currentDialogue, std::vector<std::unique_ptr<DialogueObject>> options)
        : DialogueObject(currentDialogue) {
            m_options = std::move(options);
        };
        ~ChoiceDialogue(){};
        std::unique_ptr<DialogueObject> goToNextDialogue(int choice = 0);
        std::unique_ptr<DialogueObject> makeUniquePointerOf();
        std::ostream& showCurrentDialogue(std::ostream &os) override;
    protected:
        std::vector<std::unique_ptr<DialogueObject>> m_options;
};