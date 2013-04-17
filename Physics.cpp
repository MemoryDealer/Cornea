//================================================//

#include "Physics.hpp"

//================================================//

// Tick callback 
static void tickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	Physics* physics = static_cast<Physics*>(world->getWorldUserInfo());
	physics->processTickCallback(timeStep);
}

//================================================//

Physics::Physics(void)
{
	m_cameraActive = false;
}

//================================================//

Physics::~Physics(void)
{

}

//================================================//

void Physics::init(void)
{
	const int sweep = 10000;
	m_broadphase = new btAxisSweep3(btVector3(-sweep, -sweep, -sweep), btVector3(sweep, sweep, sweep));
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_collisionWorld = new btCollisionWorld(m_collisionDispatcher, m_broadphase, m_collisionConfiguration);
	m_objectCount = 0;

	// Init tick callback
	m_world->setInternalTickCallback(tickCallback, static_cast<void*>(this), false);
}

//================================================//

void Physics::free(void)
{
	btCollisionObjectArray& objects = m_world->getCollisionObjectArray();
	int size = objects.size();
	for(int i=(size - 1); i>=0; --i){
		printf("Removing %s...\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
		m_world->removeCollisionObject(objects.at(i));
	}

	m_objects.clear();
	delete m_world;
	delete m_solver;
	delete m_collisionDispatcher;
	delete m_collisionConfiguration;
	delete m_broadphase;
}

//================================================//

void Physics::initDebugDrawer(Ogre::SceneNode* root)
{
	m_debugDrawer = new BtOgre::DebugDrawer(root, m_world);
	m_world->setDebugDrawer(m_debugDrawer);
	m_debugDrawer->setDebugMode(0);
}

//================================================//

btCollisionObject* Physics::registerEntityMesh(Ogre::Entity* entity, btCollisionWorld* colWorld)
{
    // if you wish to do instancing you will have to share one
    // btTriangleMesh amongst multiple btBvhTriangleMeshShape
    // instances
    btTriangleMesh* btMesh = new btTriangleMesh();
    
    Ogre::MeshPtr mesh = entity->getMesh();
    Ogre::Mesh::SubMeshIterator j = mesh->getSubMeshIterator();


    while (j.hasMoreElements()) {
        Ogre::SubMesh* submesh = j.getNext(); 
        
        int idxStart = submesh->indexData->indexStart;
        int nIdx = submesh->indexData->indexCount;
        
       Ogre::HardwareIndexBuffer* idxBuffer 
            = submesh->indexData->indexBuffer.get();
            
        Ogre::HardwareVertexBufferSharedPtr virtBuffer;
        Ogre::VertexDeclaration* virtDecl;
        
        if (submesh->useSharedVertices) {
            virtDecl = mesh->sharedVertexData->vertexDeclaration;
            assert(mesh->sharedVertexData->vertexBufferBinding->getBufferCount() > 0);
            virtBuffer = mesh->sharedVertexData->vertexBufferBinding->getBuffer(0);
        } else {
            virtDecl = submesh->vertexData->vertexDeclaration;
            assert(submesh->vertexData->vertexBufferBinding->getBufferCount() > 0);
            virtBuffer = submesh->vertexData->vertexBufferBinding->getBuffer(0);
        }
        
        unsigned char* pVert = static_cast<unsigned char*>(virtBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

        // need to lock the buffer since vertex data resides on GPU
        // and we need synchronization
        unsigned short* sindices = NULL;
        unsigned long* lindices = NULL;
        
        if (idxBuffer->getType() == Ogre::HardwareIndexBuffer::IT_16BIT) {
            sindices = static_cast<unsigned short*>(idxBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        } else if (idxBuffer->getType() == Ogre::HardwareIndexBuffer::IT_32BIT) {
            lindices = static_cast<unsigned long*>(idxBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        } else {
            assert(true == false);
        }
        
        const Ogre::VertexElement* elm = virtDecl->findElementBySemantic(Ogre::VES_POSITION, 0);
        int offset = elm->getOffset();
        assert(elm->getType() == Ogre::VET_FLOAT3);
        
        for (int k = idxStart; k < idxStart + nIdx; k += 3) {
            unsigned int indices[3];
            btVector3 vertices[3];
            
            if (idxBuffer->getType() == Ogre::HardwareIndexBuffer::IT_16BIT) {
                for (int l = 0; l < 3; ++l) {
                    indices[l] = sindices[k + l];
                }
            } else {
                for (int l = 0; l < 3; ++l) {
                    indices[l] = lindices[k + l];
                }
            }
            
            for (int l = 0; l < 3; ++l) { // for each vertex
                Ogre::Real* posVert = (Ogre::Real*)(pVert + indices[l] * virtBuffer->getVertexSize() + offset);
                for (int m = 0; m < 3; ++m) { // for each vertex component
                    vertices[l][m] = posVert[m];
                }
            }
            
            btMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
        }
            
        idxBuffer->unlock();
        virtBuffer->unlock();
    }
    
    btBvhTriangleMeshShape* btMeshShape = new btBvhTriangleMeshShape(btMesh, true, true);
    
    btCollisionObject* btObj = new btCollisionObject();
    btObj->setCollisionShape(btMeshShape);
	btObj->setUserPointer((void*)entity->getParentSceneNode());
    
	// Changed these to use the _getDerivedX so the collision objects are properly implemented
    Ogre::Vector3 pos = entity->getParentSceneNode()->_getDerivedPosition();
    Ogre::Quaternion orient = entity->getParentSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 scale = entity->getParentSceneNode()->_getDerivedScale();
    
    btMeshShape->setLocalScaling(btVector3(scale[0], scale[1], scale[2]));
    btMeshShape->setMargin(0.0);
    
    btTransform btTrans;
    btTrans.setIdentity();
    btTrans.setOrigin(btVector3(pos[0], pos[1], pos[2]));
    btTrans.setRotation(btQuaternion(orient[1], orient[2], orient[3], orient[0]));
    btObj->setWorldTransform(btTrans);
        
    // the last 2 parameters are bit strings representing group membership
    // and the groups which it is allowed to collide with
    colWorld->addCollisionObject(btObj, 2, 1);

	return btObj;

	// DEBUG
	//entity->setMaterialName("blue");
	//entity->setCastShadows(true);
}

//================================================//

btCollisionObject* Physics::registerEntityAABB(Ogre::Entity* entity, btCollisionWorld* colWorld)
{
	Ogre::Vector3 size = Ogre::Vector3::ZERO;
	Ogre::AxisAlignedBox aabb = entity->getBoundingBox();
	Ogre::Vector3 pos = entity->getWorldBoundingBox(true).getCenter(); // This will compenstate for origin offsets in the actual mesh file (must pass true if adding before it's updated)
    Ogre::Quaternion orient = entity->getParentSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 scale = entity->getParentSceneNode()->_getDerivedScale();

	size = aabb.getHalfSize() * 0.98f; // Ogre AABB slightly larger than Bullet's

	// Setup collision shape
	btVector3 halfExtents(size.x, size.y, size.z);
	btCollisionShape* box = new btBoxShape(halfExtents);

	box->setLocalScaling(btVector3(scale[0], scale[1], scale[2]));
	box->setMargin(0.0);

	// Setup collision object
	btCollisionObject* obj = new btCollisionObject();
	obj->setCollisionShape(box);
	obj->setUserPointer((void*)entity->getParentSceneNode());

	btTransform btTrans;
    btTrans.setIdentity();
    btTrans.setOrigin(btVector3(pos[0], pos[1], pos[2]));
    btTrans.setRotation(btQuaternion(orient[1], orient[2], orient[3], orient[0]));
    obj->setWorldTransform(btTrans);

	colWorld->addCollisionObject(obj, 2, 1);

	return obj;
}

//================================================//

btCollisionObject* Physics::registerEntityCylinder(Ogre::Entity* entity, btCollisionWorld* colWorld)
{
	Ogre::Vector3 size = Ogre::Vector3::ZERO;
	Ogre::AxisAlignedBox aabb = entity->getBoundingBox();
	Ogre::Vector3 pos = entity->getWorldBoundingBox(true).getCenter(); // This will compenstate for origin offsets in the actual mesh file
    Ogre::Quaternion orient = entity->getParentSceneNode()->_getDerivedOrientation();
    Ogre::Vector3 scale = entity->getParentSceneNode()->_getDerivedScale();

	size = aabb.getHalfSize() * 0.95f;

	// Setup collision shape
	btVector3 halfExtents(size.x, size.y, size.z);
	btCollisionShape* cyl = new btCylinderShape(halfExtents);

	cyl->setLocalScaling(btVector3(scale[0], scale[1], scale[2]));
	cyl->setMargin(0.0);

	// Setup collision object
	btCollisionObject* obj = new btCollisionObject();
	obj->setCollisionShape(cyl);
	obj->setUserPointer((void*)entity->getParentSceneNode());

	btTransform btTrans;
    btTrans.setIdentity();
    btTrans.setOrigin(btVector3(pos[0], pos[1], pos[2]));
    btTrans.setRotation(btQuaternion(orient[1], orient[2], orient[3], orient[0]));
    obj->setWorldTransform(btTrans);

	colWorld->addCollisionObject(obj, 2, 1);

	return obj;
}

//================================================//

void Physics::registerAllEntitiesInScene(Ogre::SceneManager* mgr)
{
	//BtOgre::registerAllEntitiesAsColliders(mgr, m_world);
	Ogre::SceneManager::MovableObjectIterator 
        i = mgr->getMovableObjectIterator("Entity");

    while(i.hasMoreElements()) {
        Ogre::Entity* entity = static_cast<Ogre::Entity*>(i.getNext());

		printf("Processing %s\n", entity->getParentSceneNode()->getName().c_str());

		Ogre::StringUtil strUtil;
		if(strUtil.startsWith(entity->getParentSceneNode()->getName(), "$", false)){
			switch(entity->getParentSceneNode()->getName()[1]){
			// Add complete mesh when '$$' is present
			case '$':
				registerEntityMesh(entity, m_world);
				break;

			// Add a cylinder shape
			case '@':
				registerEntityCylinder(entity, m_world);
				break;

			// Add a rigid body
			case '*':

				break;

			// Add a box shape
			default:
				registerEntityAABB(entity, m_world);
				break;
			}
		}
		else if(strUtil.startsWith(entity->getParentSceneNode()->getName(), "Placeholder_", false)){
			// This is just a placeholder used in Blender so it will just be removed from the world
			Ogre::SceneNode* node = entity->getParentSceneNode();
			node->getCreator()->destroySceneNode(node);
		}
    }
}

//================================================//

btCollisionObject* Physics::registerSceneNode(Ogre::SceneNode* node)
{
	Ogre::Entity* entity = static_cast<Ogre::Entity*>(node->getAttachedObject(0));

	if(entity != nullptr){
		switch(node->getName()[1]){
		// Add complete mesh when '$$' is present
		case '$':
			return this->registerEntityMesh(entity, m_world);
			
		// Add a cylinder shape
		case '@':
			return this->registerEntityCylinder(entity, m_world);
			
		// Add a rigid body
		case '*':

			break;

		// Add a box shape
		default:
			return this->registerEntityAABB(entity, m_world);
		}
	}

	return nullptr;
}

//================================================//

void Physics::setCameraData(CAMERA_DATA* data)
{
	m_pCameraData = data;
	m_world->addRigidBody(m_pCameraData->body);
	m_cameraActive = true;
}

//================================================//

void Physics::removeCameraBody(void)
{
	m_world->removeRigidBody(m_pCameraData->body);
	m_cameraActive = false;
}

//================================================//

void Physics::update(double timeSinceLastFrame)
{
	const float rate = 1.0f/240.0f;		// More efficient, suggested by Bullet creator Erwin
	float physicsTime = timeSinceLastFrame / 1000.0f;
	int nMaxSteps = physicsTime/(rate)+1;
	m_world->stepSimulation(physicsTime, nMaxSteps, rate);

	// Step the world
	/*const double timeStep = 1.0f/60.0f;
	static double accumulator = 0.0f;

	accumulator += timeSinceLastFrame;
	while(accumulator >= timeStep){
		m_world->stepSimulation(timeStep);
		accumulator -= timeStep;
	}*/

	// Update camera rigid body
	if(m_cameraActive){
		btTransform transform;
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(m_pCameraData->body->getUserPointer());
		m_pCameraData->body->getMotionState()->getWorldTransform(transform);

		btVector3 pos = transform.getOrigin(); //m_pCameraData->body->getCenterOfMassPosition();
		node->setPosition(
			Ogre::Vector3((float)pos[0], (float)pos[1] + *(m_pCameraData->heightOffset), (float)pos[2]));

		btQuaternion orientation = transform.getRotation();
		node->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}

	// Update debug drawer
	m_debugDrawer->step();

	// Update other rigid bodies
	btRigidBody* body;

	for(std::vector<btRigidBody*>::iterator itr = m_objects.begin(); itr != m_objects.end(); ++itr){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>((*itr)->getUserPointer());
		body = *itr;

		btVector3 pos = body->getCenterOfMassPosition();
		node->setPosition((float)pos[0], (float)pos[1], (float)pos[2]);

		btQuaternion orientation = body->getOrientation();
		node->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}
}

//================================================//

void Physics::processTickCallback(btScalar timeStep)
{
	// Limit the maximum speed of the player
	if(m_cameraActive){
		btVector3 velocity = m_pCameraData->body->getLinearVelocity();
		btScalar speed = velocity.length();
		btScalar max = *(m_pCameraData->shift) ? *(m_pCameraData->sprintVelocity) : *(m_pCameraData->maxVelocity);
		if(speed > max){
			velocity *= max/speed;
			m_pCameraData->body->setLinearVelocity(velocity);
		}
	}
}

//================================================//