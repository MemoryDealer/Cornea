//================================================//

#ifndef __WEAPON_HPP__
#define __WEAPON_HPP__

//================================================//

#include "Projectile.hpp" // Everything needed for the Weapon class is in Projectile.hpp

//================================================//

#define GENERAL_ANIMATION	"General_Animation"
#define ATTACK_ANIMATION	"Attack_Animation"
#define RELOAD_ANIMATION	"Reload_Animation"

//================================================//

class ProjectileManager;

//================================================//

/* Abstract class for any weapon in the game */
class Weapon
{
public:
	Weapon(void){}
	Weapon(Ogre::SceneManager* mgr);
	virtual ~Weapon(void);

	virtual void init(Ogre::SceneNode* parentNode);
	virtual void createAnimations(void) = 0;

	// Weapon functions
	virtual void attack(Sparks::Camera* camera);
	virtual void attackAlt(Sparks::Camera* camera){}
	virtual void attackAlt2(Sparks::Camera* camera){}
	virtual void reload(void){}

	// Debug/misc. functions
	void drawDebugRay(Ogre::Vector3 from, Ogre::Vector3 to);

	// Getter functions
	const unsigned getType(void) const;
	const Ogre::Entity* getEntity(void) const;
	Ogre::SceneNode* getSceneNode(void) const;
	const Ogre::Vector3 getPosition(void) const;
	const Ogre::Vector3 getOffset(void) const;
	Projectile::PROJECTILE_DATA getProjectileData(void) const;
	const Ogre::String getReticuleMaterialName(unsigned type) const;
	const Ogre::Real getRange(void) const;
	const Ogre::Real getReticuleRange(void) const;

	// Setter functions
	void setOffset(Ogre::Vector3 offset);
	void setProjectileManager(ProjectileManager* mgr);
	void setNPCManager(NPCManager* mgr);

	// Update
	virtual void update(double timeSinceLastFrame) = 0;

	// --- //

	enum{
		TYPE_RAY = 0,
		TYPE_PROJECTILE,
		TYPE_MELEE
	};

	enum{
		RETICULE_DEFAULT = 0,
		RETICULE_ENEMY,
		RETICULE_FRIENDLY
	};

	enum{
		STATE_IDLE = 0,
		STATE_MOVING,
		STATE_ATTACKING,
		STATE_RELOADING
	};

	// Weapon definitions
	enum{
		NONE = 0,
		TEST_GUN,
		TEST_SWORD
	};

protected:
	bool rayAttack(Sparks::Camera* camera);
	bool projectileAttack(Sparks::Camera* camera);
	bool meleeAttack(Sparks::Camera* camera);

	// --- //

	Ogre::SceneManager*	m_pSceneMgr;
	Ogre::Entity*		m_pEntity;
	Ogre::SceneNode*	m_pSceneNode;
	Ogre::SceneNode*	m_pParentNode;
	Ogre::String		m_filename;

	Ogre::Vector3		m_defPosition;
	Ogre::Quaternion	m_defOrientation;
	Ogre::Vector3		m_offset;

	// Main animation state
	Ogre::AnimationState* m_pAnimState;

	// Effects
	ImpactManager*		m_impactManager;
	MuzzleFlash*		m_muzzleFlash;
	Ogre::Vector3		m_muzzleFlashPos;
	unsigned			m_muzzleFlashType;

	// Sound
	Sound*				m_pSoundAttack;
	Sound*				m_pSoundMelee;
	Sound*				m_pSoundMeleeHit;
	Sound*				m_pSoundReload;


	// Weapon behaviour
	unsigned			m_type;
	unsigned			m_state;
	bool				m_isLoaded;
	Ogre::Vector3		m_projectileOffset; // where the projectile should exit
	Ogre::Real			m_damage;
	Ogre::Real			m_range;
	Ogre::Real			m_reticuleRange;

	ProjectileManager*  m_projectileManager;
	Projectile::PROJECTILE_DATA		m_projectileData;
	NPCManager*			m_pNPCManager;

	// Crosshair
	Ogre::String		m_reticuleDefault;
	Ogre::String		m_reticuleEnemy;
	Ogre::String		m_reticuleFriendly;

	// Debug
	Ogre::SceneNode*	m_lineNode;
	Ogre::ManualObject* m_manualObject;
	bool				m_debugRays;
};

//================================================//

// Getter functions

inline const unsigned Weapon::getType(void) const
{ return m_type; }

inline const Ogre::Entity* Weapon::getEntity(void) const
{ return m_pEntity; }

inline Ogre::SceneNode* Weapon::getSceneNode(void) const
{ return m_pSceneNode; }

inline const Ogre::Vector3 Weapon::getPosition(void) const
{ return m_pSceneNode->_getDerivedPosition(); }

inline const Ogre::Vector3 Weapon::getOffset(void) const
{ return m_offset; }

inline Projectile::PROJECTILE_DATA Weapon::getProjectileData(void) const
{ return m_projectileData; }

inline const Ogre::String Weapon::getReticuleMaterialName(unsigned type) const
{ 
	return (type == Weapon::RETICULE_DEFAULT) ? m_reticuleDefault :
	(type == Weapon::RETICULE_ENEMY) ? m_reticuleEnemy : m_reticuleFriendly; 
}

inline const Ogre::Real Weapon::getRange(void) const
{ return m_range; }

inline const Ogre::Real Weapon::getReticuleRange(void) const
{ return m_reticuleRange; }

// Setter functions
inline void Weapon::setProjectileManager(ProjectileManager* mgr)
{ m_projectileManager = mgr; }

inline void Weapon::setNPCManager(NPCManager* mgr)
{ m_pNPCManager = mgr; }

//================================================//
//================================================//

class WeaponNone : public Weapon
{
public:
	WeaponNone(Ogre::SceneManager* mgr);

	void init(Ogre::SceneNode* node);
	void createAnimations(void);
	void attack(Sparks::Camera* camera){}
	void attackAlt(Sparks::Camera* camera){}

	void update(double timeSinceLastFrame);

private:

};

//================================================//
//================================================//

#endif

//================================================//