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

	// Most of these settings can be public--hacking these won't affect gameplay

	// Graphics
	struct graphics_t{
		
		// Shadows
		struct shadows_t{
			bool	enabled;
			int		fsaa;
			int		textureCount;
			int		textureSize;
		} shadows;

		bool useSkyX;
		bool useHydraX;
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
private:

};

//================================================//

#endif

//================================================//