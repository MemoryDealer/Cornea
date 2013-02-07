//================================================//

#include "Profile.hpp"

//================================================//

Profile::Profile(void)
{
	m_pData = new PROFILE_DATA();
	memset(m_pData, 0, sizeof(PROFILE_DATA));

	this->setDefaults();
}

//================================================//

Profile::~Profile(void)
{
	delete m_pData;
	m_pData = 0;
}

//================================================//

void Profile::setDefaults(void)
{
	m_pData->magic = BINARY_MAGIC;
	m_pData->name = "Default";
	m_pData->difficulty = NORMAL;
	m_pData->stage = 0;

	m_pData->inventory.setDefaults();
}

//================================================//

bool Profile::create(Ogre::String name)
{
	this->setDefaults();
	
	m_pData->name = name;
	
	if(save()){
		return true;
	}
	
	return false;
}

//================================================//

bool Profile::load(Ogre::String name)
{
	// Add encryption

	this->setDefaults();

	std::ifstream fp("Saves/" + name, std::ifstream::binary);

	if(fp.is_open()){
		// Compare sizes
		fp.seekg(0, std::ios::end);
		int size = (int)fp.tellg();
		fp.seekg(0, std::ios::beg);
		
		if(size == sizeof(PROFILE_DATA)){
			fp.read(reinterpret_cast<char*>(m_pData), sizeof(PROFILE_DATA));
			fp.close();

			Ogre::StringUtil strUtil;
			if(strUtil.match(m_pData->magic, BINARY_MAGIC, true)){
				printf("Matched!!\n");
				return true;
			}
		}
		else{
			printf("Invalid size! (%d)\n", size);
		}
	}

	return false;
}

//================================================//

bool Profile::save(void)
{
	// Create Saves directory if not already there
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	(void)CreateDirectory("Saves", NULL);
#endif

	std::ofstream fp("Saves/" + m_pData->name + ".save", std::ofstream::binary);

	if(fp.is_open()){
		fp.write(reinterpret_cast<const char*>(m_pData), sizeof(PROFILE_DATA));
		fp.close();


		//! More error handling
		return true;
	}

	return false;
}

//================================================//