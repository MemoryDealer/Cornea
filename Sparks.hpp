//================================================//

#ifndef __SPARKS_HPP__
#define __SPARKS_HPP__

//================================================//

#include "stdafx.h"

//================================================//

namespace Sparks
{

//================================================//

// functions for parent nodes of DotSceneLoader's nodes
void translateDotSceneNode(Ogre::SceneNode* node, Ogre::Vector3 vec);
void scaleDotSceneNode(Ogre::SceneNode* node, Ogre::Vector3 vec);
void rotateDotSceneNode(Ogre::SceneNode* node, Ogre::Quaternion q);
void setDotSceneNodePosition(Ogre::SceneNode* node, Ogre::Vector3 vec);

// Misc. scene node functions
void destroyAllAttachedMovableObjects(Ogre::SceneNode* sceneNode);
Ogre::Vector3 getWorldPosition(Ogre::SceneNode* node);

// Bullet and Ogre converter functions
Ogre::Vector3 bulletToOgreVector3(btVector3& vec);
btVector3 ogreToBulletVector3(Ogre::Vector3& vec);

// collision 
void GetMeshInformation(const Ogre::MeshPtr mesh,
                                size_t &vertex_count,
                                Ogre::Vector3* &vertices,
                                size_t &index_count,
                                Ogre::uint32* &indices,
                                const Ogre::Vector3 &position,
                                const Ogre::Quaternion &orient,
                                const Ogre::Vector3 &scale);

//================================================//

} // namespace Sparks

//================================================//

#endif

//================================================//