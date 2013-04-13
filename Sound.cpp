//================================================//

#include "Sound.hpp"

//================================================//

Sound::Sound(void)
{
	m_loaded = false;
}

//================================================//

Sound::Sound(unsigned type, Ogre::Vector3 pos)
{
	m_loaded = false;

	this->init(type, pos);
}

//================================================//

Sound::~Sound(void)
{
	if(m_loaded){
		m_sound->release();
		
	}
}

//================================================//

void Sound::init(unsigned type, Ogre::Vector3 pos)
{
	const char* file;
	FMOD_VECTOR fPos, fVel;

	m_mode = 0;

	memset(&fPos, 0, sizeof(fPos));
	memset(&fVel, 0, sizeof(fVel));

	switch(type){
	case TYPE_NULL:
	default:
		return;

	case TYPE_FIRE1:
		file = "D:/Sounds/magnum.mp3";
		m_mode = FMOD_2D;
		break;

	case TYPE_RETRIEVE_MAGIC_CUBE:
		file = "D:/Sounds/click.wav";
		m_mode = FMOD_2D;
		break;


	case TYPE_MUSIC_MAIN_MENU:
		file = "D:/Sounds/menu.mp3";
		m_mode = FMOD_2D;
		break;
	}

	FMOD_RESULT res = Base::getSingletonPtr()->m_soundSystem->createSound(file, m_mode, 0, &m_sound);

	// 3D
	if(m_mode & FMOD_3D){
		fPos.x = pos.x;
		fPos.y = pos.y;
		fPos.z = pos.z;

		m_channel->set3DAttributes(&fPos, &fVel);
	}

	if(res == FMOD_OK)
		m_loaded = true;
	else
#ifdef _DEBUG
		printf("Failed to load sound %s\n", file);
#endif
}

//================================================//

void Sound::play(Ogre::Vector3 pos)
{
	if(m_loaded)
		Base::getSingletonPtr()->m_soundSystem->playSound(FMOD_CHANNEL_FREE, m_sound, false, &m_channel);
}

//================================================//

void Sound::updatePos(Ogre::Vector3 pos, Ogre::Vector3 vel)
{
	if(m_mode & FMOD_3D){
		FMOD_VECTOR fPos, fVel;
		fPos.x = pos.x;
		fPos.y = pos.y;
		fPos.z = pos.z;

		fVel.x = vel.x;
		fVel.y = vel.y;
		fVel.z = vel.z;

		m_channel->set3DAttributes(&fPos, &fVel);
	}
}

//================================================//

void Sound::update(double timeSinceLastFrame)
{

}

//================================================//

