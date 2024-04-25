#ifndef AUDIO_H_
#define AUDIO_H_

#include <SFML/Audio/SoundBuffer.hpp>

/**
 * @brief Plays a sound from a buffer
 * @attention Run this function in a thread
 * 
 * @param buffer The sound buffer 
 */
void playSound(sf::SoundBuffer buffer);

#endif //AUDIO_H_