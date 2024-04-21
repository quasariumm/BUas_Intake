#ifndef DIALOGUE_H_
#define DIALOGUE_H_

#include <string>
#include <utility>
#include <vector>

#include "../include/ui_conf.hpp"

class TextBubble : public UIElements::TextLabel {
public:

  /**
   * @brief Construct a new Text Bubble object
   * 
   * @param text The text that the narrator should say
   */
  TextBubble(const std::string text);

  /**
   * @brief Set the Message object
   * 
   * @param newMessage The new message
   */
  void setMessage(const std::string newMessage) {message = newMessage;};

  /**
   * @brief Gradually make the text appear.
   * @attention This function needs to be run in a thread
   * 
   */
  void typewriterText();

  /**
   * @brief 
   * 
   */
  void draw();

private:

  std::string message;

};

class Dialogue {
public:

  /**
   * @brief Loads dialogue instructions from a dialogue file
   * 
   * @param dialogueFile The dialogue file (*.qd)
   */
  void loadFromFile(const std::filesystem::path dialogueFile);
  
  /**
   * @brief Plays the dialogue
   * @attention This function needs to be run in a thread
   * 
   * @param textBubble The text bubble to print the dialogue on
   */
  void play(TextBubble* textBubble);

private:

  std::vector<std::pair<std::string, std::string>> instructions;

};

#endif //DIALOGUE_H_