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
	~Sound(void);

	void init(unsigned type, Ogre::Vector3 pos = Ogre::Vector3::ZERO);

	void play(Ogre::Vector3 pos = Ogre::Vector3::ZERO);
	void stop(void);

	void updatePos(Ogre::Vector3 pos, Ogre::Vector3 vel = Ogre::Vector3::ZERO);
	void update(double timeSinceLastFrame);

	// --- //

	enum{
		TYPE_NULL = 0,
		TYPE_FIRE1
	};

protected:
	FMOD::Channel*		m_channel;
	FMOD::Sound*		m_sound;
	FMOD_MODE			m_mode;

	bool				m_loaded;
};

//================================================//

#endif

//================================================//