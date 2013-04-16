//================================================//

#ifndef __SETTINGS_HPP__
#define __SETTINGS_HPP__

//================================================//

#include "stdafx.h"

//================================================//

/* This is loaded at the start of the application can be accessed from any file */
class Settings : public Ogre::Singleton<Settings>
{
public:
	Settings(void);
	~Settings(void);

	void setPreset(int preset);
	void setDefaultControls(void);

	static const int KEYMAP_SIZE = 100;

	// Graphics
	struct graphics_t{
		
		// Shadows
		struct shadows_t{
			bool	enabled;
			int		fsaa;
			int		textureCount;
			int		textureSize;
		} shadows;

		int sky;
		int water;
		Ogre::Real farClip;
	} graphics;

	// Audio
	struct audio_t{

		// Music
		struct music_t{

		} music;

		// Sound
		struct sound_t{

		} sound;
	} audio;

	// Controls
	struct controls_t{
		unsigned short keys[KEYMAP_SIZE];
	} controls;

	// Key binding enums
	enum KEY{
		ZERO = 0,
		MOVE_FORWARD,
		MOVE_LEFT,
		MOVE_BACKWARD,
		MOVE_RIGHT,
		ACTION,
	};

	// Preset enums
	enum{
		LOW = 0,
		MEDIUM,
		HIGH,
		ULTRA
	};

private:
	
};

//================================================//

#endif

//================================================//