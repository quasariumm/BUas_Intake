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
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

#include "../include/globals.hpp"

void playSound(sf::SoundBuffer buffer) {
  sf::Sound sound(buffer);

  sound.setVolume(Globals::volume);
  sound.play();

  while (sound.getStatus() == sf::Sound::Playing) {
    sf::sleep(sf::milliseconds(10));
  }
}