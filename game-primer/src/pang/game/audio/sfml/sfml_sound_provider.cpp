// sfml_sound_provider.cpp
// the SFML engine of sound provider

#include "sfml_sound_provider.h"
#include "exceptions/exceptions.h"
#include <iostream>

SFMLSoundProvider::SFMLSoundProvider()
: _currentSongName("") { }

void SFMLSoundProvider::playSound(std::string filename) {
    int availChannel = -1;
    for (int i = 0; i < MAX_SOUND_CHANNELS; i++) {
        if(_currentSounds[i].getStatus() != sf::Sound::Playing) {
            availChannel = i;
            break;
        }
    }
    
    // if all sound channels are in use, do nothing for now
    if(availChannel != -1) {
        try {
            _currentSounds[availChannel] = _soundFileCache.getSound(filename);
            _currentSounds[availChannel].play();
        }
        catch(SoundNotFoundExeception& snfe) {
			std::cerr << snfe.what() << std::endl;
        }
    }
}

void SFMLSoundProvider::playSong(std::string filename, bool looping) {
    sf::Music* currentSong;
    try {
        currentSong = _soundFileCache.getSong(filename);
    }
    catch (SoundNotFoundExeception &) {
        // This one is dire, means we couldn't find or load the selected song
        // So, lets exit!
        return;
    }
    // See if prior song is playing still, if so, stop it
    if (_currentSongName != "") {
        try {
            sf::Music* priorSong = _soundFileCache.getSong(_currentSongName);
            if(priorSong->getStatus() != sf::Sound::Stopped) {
                priorSong->stop();
            }
        }
        catch(SoundNotFoundExeception&) {
            // Do nothing, this exception isn't dire.  It simply means the previous sound we were
            // trying to stop wasn't located.
        }
        
    }
    _currentSongName = filename;
    currentSong->setLoop(looping);
    currentSong->play();
}

void SFMLSoundProvider::stopAllSounds() {
    for(int i = 0; i < MAX_SOUND_CHANNELS; i++) {
        _currentSounds[i].stop();
    }
    
    if(_currentSongName != "") {
        sf::Music * currentSong = _soundFileCache.getSong(_currentSongName);
        if(currentSong->getStatus() == sf::Sound::Playing) {
            currentSong->stop();
        }
    }
}

bool SFMLSoundProvider::isSoundPlaying() {
    for(int i = 0; i < MAX_SOUND_CHANNELS; i++) {
        if(_currentSounds[i].getStatus() == sf::Sound::Playing)
            return true;
    }
    return false;
}


bool SFMLSoundProvider::isSongPlaying() {
    if(_currentSongName != "") {
        return _soundFileCache.getSong(_currentSongName)->getStatus() == sf::Music::Playing;
    }
    return false;
}
