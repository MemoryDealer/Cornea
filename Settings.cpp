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
		
	}
	
	// Temp. settings
	graphics.shadows.enabled = true;
	graphics.shadows.fsaa = 4;
	graphics.shadows.textureCount = 4;
	graphics.shadows.textureSize = 256;
}

//================================================//

Settings::~Settings(void)
{

}

//================================================//