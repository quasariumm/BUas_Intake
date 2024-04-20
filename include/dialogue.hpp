#ifndef DIALOGUE_H_
#define DIALOGUE_H_

#include <string>

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
   * @brief Gradually make the text appear.
   * @attention This function has to be threaded
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

#endif //DIALOGUE_H_