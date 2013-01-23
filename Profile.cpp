//================================================//

#include "Profile.hpp"

template<> Profile* Ogre::Singleton<Profile>::msSingleton = 0;

//================================================//

Profile::Profile(void)
{

}

//================================================//

Profile::~Profile(void)
{

}

//================================================//

bool Profile::load(Ogre::String filename)
{

	return false;
}

//================================================//

bool Profile::save(void)
{

	return false;
}

//================================================//