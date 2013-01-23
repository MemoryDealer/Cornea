//================================================//

#include "WarpGun.hpp"

//================================================//

WarpGun::WarpGun(Ogre::SceneManager* mgr) : Weapon(mgr)
{
	m_pSceneMgr = mgr;

	// filename for mesh
	m_filename = "br.mesh";

	// offset for weapon placement (not resolution independent)
	m_offset = Ogre::Vector3(2.50, -2.10, -7.20);

	// behavior
	m_type = TYPE_PROJECTILE;
	m_projectileOffset = Ogre::Vector3(0, 0, -1.0);

	// projectile data
	m_projectileData.colour = Ogre::ColourValue(1.0, 1.0, 1.0);
	m_projectileData.speed = 1.0;
	m_projectileData.range = 2000.0;
}

//================================================//

WarpGun::~WarpGun(void)
{
	m_pSceneMgr->destroyAnimation(ATTACK_ANIMATION);
}

//================================================//

void WarpGun::init(Ogre::SceneNode* node)
{
	Weapon::init(node);

	m_pSceneNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Radian((Ogre::Math::PI / 2.0)), Ogre::Node::TS_LOCAL);
	m_pSceneNode->scale(1.6, 1.6, 1.6);

	m_pEntity->setMaterialName("mainbody");
	m_pEntity->setCastShadows(false);

	createAnimations();
}

//================================================//

void WarpGun::createAnimations(void)
{
	m_pSceneNode->setInitialState();

	Ogre::Animation* anim = m_pSceneMgr->createAnimation(ATTACK_ANIMATION, 500.0);
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	
	anim->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	// was having issues with the animation resetting properly each time

	Ogre::TransformKeyFrame* key;
	key = track->createNodeKeyFrame(0.0);

	key = track->createNodeKeyFrame(250.0);
	key->setTranslate(Ogre::Vector3(0.0, 0.0, 3.0));

	key = track->createNodeKeyFrame(500.0);
	key->setTranslate(Ogre::Vector3::ZERO);

	m_pAnimState = m_pSceneMgr->createAnimationState(ATTACK_ANIMATION);
	m_pAnimState->setLoop(false);
}

//================================================//

void WarpGun::attack(Sparks::Camera* camera)
{
	if(!m_pAnimState->getEnabled()){
		// For the Warp Gun, only allow one projectile at a time
		if(m_projectileManager->getCount() >= 1)
			return;

		m_pAnimState->setEnabled(true);

		m_state = STATE_ATTACKING;

		// Add projectile to the manager
		m_projectileData.origin = Weapon::getPosition() + m_projectileOffset;
		m_projectileData.direction = camera->getDirection();

		m_projectileManager->addProjectile(m_projectileData);
	}
}

//================================================//

void WarpGun::attackAlt(Sparks::Camera* camera)
{
	if(m_projectileManager->getCount() == 0)
		return;

	Ogre::Vector3 pos = m_projectileManager->getProjectilePosition(0);
	m_projectileManager->clearAll();

	camera->setPosition(pos);
}

//================================================//

void WarpGun::update(double timeSinceLastFrame)
{
	// For weapons like this, just completely override the update function to control the shot timing
	Weapon::update(timeSinceLastFrame);

	switch(m_state){
	case STATE_IDLE:
	default:
		return;
	}
}

//================================================//