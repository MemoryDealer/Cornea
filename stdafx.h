//================================================//

#ifndef __STDAFX_H__
#define __STDAFX_H__

//================================================//

// Ogre
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <Plugins/OctreeSceneManager/OgreOctreeSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreMeshManager.h>
#include <OgreParticleSystem.h>
#include <OgreConfigFile.h>
#include <OgreWindowEventUtilities.h>
#include <OgreString.h>
#include <OgreStringConverter.h>
#include <OgreException.h>
#include <OgreResourceManager.h>
#include <OgreCompositor.h>
#include <OgreCompositorManager.h>
#include <OgreCompositionPass.h>
#include <OgreCompositionTargetPass.h>

//================================================//

// OIS input
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//================================================//

// MyGUI
#include <MyGUI.h>
#include <MyGUI_OgrePlatform.h>

//================================================//

// SkyX
#include "SkyX/SkyX.h"

//================================================//

// Hydrax
#include "Hydrax/Hydrax.h"
#include "Hydrax/Noise/Perlin/Perlin.h"
#include "Hydrax/Modules/ProjectedGrid/ProjectedGrid.h"
#include "Hydrax/Modules/SimpleGrid/SimpleGrid.h"
#include "Hydrax/Modules/RadialGrid/RadialGrid.h"

//================================================//

// Bullet Physics
#include <btDefaultCollisionConfiguration.h>
#include <btCollisionDispatcher.h>
#include <btCollisionWorld.h>
#include <btAxisSweep3.h>
#include <btBroadphaseInterface.h>
#include <btSequentialImpulseConstraintSolver.h>
#include <btDiscreteDynamicsWorld.h>
#include <btRigidBody.h>
#include <btTransform.h>
#include <btDefaultMotionState.h>
#include <btStaticPlaneShape.h>
#include <btBoxShape.h>
#include <btSphereShape.h>
#include <btStridingMeshInterface.h>
#include <btBvhTriangleMeshShape.h>
#include <btBulletCollisionCommon.h>

//================================================//

// Sound
#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h>

//================================================//

// STL headers etc.
#include <map>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <stack>

//================================================//

// Game
//#include "Settings.hpp"

//================================================//

#endif