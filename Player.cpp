//================================================//

#include "Player.hpp"

//================================================//

Player::Player(Ogre::SceneManager* mgr, Profile* profile)
{
	// Set the scene manager pointer
	m_pSceneMgr = mgr;

	m_pProfile = profile;

	// Create the camera
	m_pCamera = new Sparks::Camera(m_pSceneMgr, 90000);
	m_pCamera->setMode(Sparks::Camera::MODE_FIRST_PERSON);

	// Set weapon to NULL for now
	m_nWeapon = Weapon::NONE;
	m_pWeapon = nullptr;

	// How close a player should be to trigger an action
	m_actionRange = 45.0;

	// Load sounds
	m_pSoundRetrieve = new Sound(Sound::TYPE_RETRIEVE_MAGIC_CUBE);
}

//================================================//

Player::~Player(void)
{
	if(m_pCamera != nullptr) delete m_pCamera;
	if(m_pWeapon != nullptr) delete m_pWeapon;
	if(m_pFlashlight != nullptr) delete m_pFlashlight;

	// Free sounds
	delete m_pSoundRetrieve;
}

//================================================//

void Player::spawn(Ogre::SceneNode* node)
{
	this->setPosition(node->_getDerivedPosition());

	// Rotate to face the direction of the cone's point
	m_pCamera->getYawNode()->yaw(Ogre::Radian((Ogre::Math::PI / 2.0)));

	node->detachAllObjects();
}

//================================================//

void Player::action(EventManager* eventManager)
{
	Rayhit* rayhit = m_pCamera->getNegativeZRayhit();

	if(rayhit->hasHit){
		printf("Object hit %s\nDistance: %.2f\n", rayhit->node->getName().c_str(), rayhit->distance);

		// Iterate through dynamic objects
		std::vector<DynamicObject*> objects = eventManager->getDynamicObjectManager()->getObjects();

		for(std::vector<DynamicObject*>::iterator itr = objects.begin();
			itr != objects.end();
			++itr){
			if((*itr)->getSceneNode() == rayhit->node){
				if(rayhit->distance <= m_actionRange){
					(*itr)->send(DynamicObject::ARG_ACTION);

					if((*itr)->isRetrievable()){
						(*itr)->retrieve();
						this->processRetrieval((*itr));
					}
					break;
				}
			}
		}
	}
}

//================================================//

void Player::processRetrieval(DynamicObject* object)
{
	printf("Typeid: %s\n", typeid(*object).name());

	// Determine type of object
	if(typeid(*object) == typeid(MagicCube)){
		m_pProfile->getInventory().addMagicCube();
		m_pSoundRetrieve->play();
		
		// Add text
		Text* text = new Text();
		text->text = "Magic Cube Acquired";
		text->font = "StarWars";
		text->pos = TextRenderer::LOWER;
		text->timeout = 2000;

		TextRenderer::getSingletonPtr()->setText(text);
	}
}

//================================================//

void Player::initFlashlight(void)
{
	m_pFlashlight = new Flashlight(m_pSceneMgr, m_pCamera->getRollNode());
	//m_pFlashlight = new Flashlight(m_pSceneMgr, m_pCamera->getSceneNode());
}

//================================================//

void Player::initGlasses(Sparks::Compositor* compositor)
{
	m_pGlasses = new Glasses(compositor);
}

//================================================//

void Player::setWeapon(unsigned weapon, EventManager* pEventManager)
{
	if(weapon != Weapon::NONE){
		if(m_nWeapon == weapon){
			return;
		}
	}

	// Set new weapon and delete the old class
	m_nWeapon = weapon;
	if(m_pWeapon != nullptr)
		delete m_pWeapon;

	// Load the new derived class for the weapon
	switch(m_nWeapon){
	case Weapon::NONE:
	default:
		m_pWeapon = new WeaponNone(m_pSceneMgr);
		break;

	case Weapon::TEST_GUN:
		m_pWeapon = new MLP(m_pSceneMgr);
		break;

	case Weapon::TEST_SWORD:
		m_pWeapon = new Sword(m_pSceneMgr);
		break;
	}

	// Initialise the weapon
	m_pWeapon->init(m_pCamera->getRollNode());
	//m_pWeapon->init(m_pCamera->getSceneNode());

	// Set manager pointers
	m_pWeapon->setNPCManager(pEventManager->getNPCManager());
	m_pWeapon->setProjectileManager(pEventManager->getProjectileManager());
}

//================================================//

void Player::update(double timeSinceLastFrame)
{
	// Update the camera
	m_pCamera->update(timeSinceLastFrame);

	// Update the player's weapon
	m_pWeapon->update(timeSinceLastFrame);

	// Update player's flashlight
	m_pFlashlight->update(timeSinceLastFrame);
}

//================================================//