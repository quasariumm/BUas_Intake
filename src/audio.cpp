/**
 * @file audio.cpp
 * @author Patrick Vreeburg
 * @brief Handles the audio
 * @version 0.1
 * @date 2024-04-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/audio.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <chrono>

#include "../include/globals.hpp"

void playSound(sf::SoundBuffer buffer) {
  sf::Sound* sound = new sf::Sound(buffer);

  sound->setVolume(Globals::volume);
  sound->play();

  while (sound->getStatus() == sf::Sound::Playing) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
  delete sound;
}