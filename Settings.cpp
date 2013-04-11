//================================================//

#include "Settings.hpp"

template<> Settings* Ogre::Singleton<Settings>::msSingleton = 0;

//================================================//

Settings::Settings(void)
{
	// Check for config file, create one if not there
	bool configFileExists = false;
	Ogre::ConfigFile cfg;
	if(configFileExists){
		cfg.loadDirect("config.ini");
		return;
	}
	
	// Temp. settings
	graphics.shadows.enabled = true;
	graphics.shadows.fsaa = 0;
	graphics.shadows.textureCount = 8;
	graphics.shadows.textureSize = 256;

	graphics.sky = MEDIUM;
	graphics.water = MEDIUM;

	this->setDefaultControls();
}

//================================================//

Settings::~Settings(void)
{

}

//================================================//

void Settings::setPreset(int preset)
{
	switch(preset){
	case LOW:
		graphics.sky = graphics.water = LOW;
		break;

	default:
	case MEDIUM:

		break;

	case HIGH:

		break;

	case ULTRA:

		break;
	}
}

//================================================//

void Settings::setDefaultControls(void)
{
	// Movement
	controls.keys[KEY::MOVE_FORWARD]	= OIS::KC_W;
	controls.keys[KEY::MOVE_LEFT]		= OIS::KC_A;
	controls.keys[KEY::MOVE_BACKWARD]	= OIS::KC_S;
	controls.keys[KEY::MOVE_RIGHT]		= OIS::KC_D;

	controls.keys[KEY::ACTION]			= OIS::KC_E;
}

//================================================//