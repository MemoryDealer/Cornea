//================================================//

#ifndef __SHAREDDATA_HPP__
#define __SHAREDDATA_HPP__

//================================================//

#include "stdafx.h"

//================================================//

/* A minimal use singleton for shared data between game states/files */
class SharedData : public Ogre::Singleton<SharedData>
{
public:
	SharedData(void){}
	~SharedData(void){}

	Ogre::String	buffer;
	int				action;

private:

};

//================================================//

#endif

//================================================//