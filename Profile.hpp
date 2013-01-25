//================================================//

#ifndef __PROFILE_HPP__
#define __PROFILE_HPP__

//================================================//

#define BINARY_MAGIC "$__A+A010~-~__$"

//================================================//

#include "stdafx.h"
#include "Inventory.hpp"

//================================================//

/* This functions as a "saved game" that can be loaded and created from the main menu. 
	Each one is named whatever the player calls it and stored in a binary file. */
class Profile : public Ogre::Singleton<Profile>
{
public:
	Profile(void);
	~Profile(void);

	bool create(Ogre::String name);
	bool load(Ogre::String name);
	bool save(void);

	// Getter functions
	const int getDifficulty(void) const;
	Inventory* getInventory(void) const;

	// --- //

	enum DIFFICULTY{
		CASUAL = 0,
		NORMAL,
		INSANE,
		RIDICULOUS
	};

private:
	// All sensitive data needs to be private and less hackable

	typedef struct{
		Ogre::String	magic;

		Ogre::String	name;

		int				difficulty;

		// In-game data
		Inventory*		pInventory;
		
	} PROFILE_DATA;

	PROFILE_DATA*		m_pData;
};

//================================================//

inline const int Profile::getDifficulty(void) const
{ return m_pData->difficulty; }

inline Inventory* Profile::getInventory(void) const
{ return m_pData->pInventory; }

//================================================//

#endif

//================================================//