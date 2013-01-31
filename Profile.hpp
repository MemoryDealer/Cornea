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
	const int getStage(void) const;
	Inventory* getInventory(void) const;

	// --- //

	enum DIFFICULTY{
		CASUAL = 0,
		NORMAL,
		INSANE,
		RIDICULOUS
	};

	enum STAGE{
		OIL_RIG = 0,
		END
	};

private:
	// All sensitive data needs to be private and less hackable

	typedef struct{
		Ogre::String	magic;

		Ogre::String	name;

		// Game save data
		int				difficulty;
		int				stage;

		// In-game data
		Inventory*		pInventory;
		int				health;
		
	} PROFILE_DATA;

	PROFILE_DATA*		m_pData;
};

//================================================//

inline const int Profile::getDifficulty(void) const
{ return m_pData->difficulty; }

inline const int Profile::getStage(void) const
{ return m_pData->stage; }

inline Inventory* Profile::getInventory(void) const
{ return m_pData->pInventory; }

//================================================//

#endif

//================================================//