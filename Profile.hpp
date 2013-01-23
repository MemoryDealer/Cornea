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

	bool load(Ogre::String filename);
	bool save(void);

	// Getter functions
	const int getDifficulty(void) const;

	// --- //

	enum DIFFICULTY{
		CASUAL = 0,
		CHALLENGING,
		INSANE,
		RIDICULOUS
	};

private:
	// All sensitive data needs to be private and less hackable
	struct game_t{
		int		difficulty;
	} game;

	typedef struct{
		Ogre::String	magic;

		Ogre::String	name;
		Inventory		inventory;
	} BINARY_FILE, *PBINARY_FILE;

	BINARY_FILE			data;
};

//================================================//

inline const int Profile::getDifficulty(void) const
{ return game.difficulty; }

//================================================//

#endif

//================================================//