//================================================//

#ifndef __SOUND_HPP__
#define __SOUND_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"

//================================================//

class Sound
{
public:
	Sound(void);
	Sound(unsigned type, Ogre::Vector3 pos = Ogre::Vector3::ZERO);
	~Sound(void);

	void init(unsigned type, Ogre::Vector3 pos = Ogre::Vector3::ZERO);

	void play(Ogre::Vector3 pos = Ogre::Vector3::ZERO);
	void setPaused(bool pause);
	void stop(void);

	void updatePos(Ogre::Vector3 pos, Ogre::Vector3 vel = Ogre::Vector3::ZERO);
	void update(double timeSinceLastFrame);

	// --- //

	enum{
		TYPE_NULL = 0,
		TYPE_FIRE1,
		TYPE_RETRIEVE_MAGIC_CUBE,

		TYPE_MUSIC_MAIN_MENU,

		TYPE_END
	};

protected:
	FMOD::Channel*		m_channel;
	FMOD::Sound*		m_sound;
	FMOD_MODE			m_mode;

	bool				m_loaded;
};

//================================================//

inline void Sound::setPaused(bool pause)
{ m_channel->setPaused(pause); }

inline void Sound::stop(void)
{ m_channel->stop(); }

//================================================//

#endif

//================================================//