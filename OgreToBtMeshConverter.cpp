//================================================//

#include "OgreToBtMeshConverter.hpp"

//================================================//

using namespace Ogre;
//
// NOTE: This code will not do any mesh instancing within Bullet. This
// should suffice for levels without much mesh duplication. If you
// start finding yourself running out of memory because you have lots
// of instances of the same mesh, you might want to look into
// extending this code to support instancing.
//

//
// This method takes in an entity and reads is mesh data into
// a btBvhTriangleMesh and places it into the collision world.
// A bit hairy since the mesh data in OGRE is stored on the GPU.
// It will place all entities into group 2 and set it to collide with
// group 1.
//

//================================================//

void registerEntityAsCollider(Entity* entity, btCollisionWorld* colWorld, EventManager* pEventManager)
{
    // if you wish to do instancing you will have to share one
    // btTriangleMesh amongst multiple btBvhTriangleMeshShape
    // instances
    btTriangleMesh* btMesh = new btTriangleMesh();
    
    MeshPtr mesh = entity->getMesh();
    Mesh::SubMeshIterator j = mesh->getSubMeshIterator();
    while (j.hasMoreElements()) {
        SubMesh* submesh = j.getNext(); 
        
        int idxStart = submesh->indexData->indexStart;
        int nIdx = submesh->indexData->indexCount;
        
        HardwareIndexBuffer* idxBuffer 
            = submesh->indexData->indexBuffer.get();
            
        HardwareVertexBufferSharedPtr virtBuffer;
        VertexDeclaration* virtDecl;
        
        if (submesh->useSharedVertices) {
            virtDecl = mesh->sharedVertexData->vertexDeclaration;
            assert(mesh->sharedVertexData->vertexBufferBinding->getBufferCount() > 0);
            virtBuffer = mesh->sharedVertexData->vertexBufferBinding->getBuffer(0);
        } else {
            virtDecl = submesh->vertexData->vertexDeclaration;
            assert(submesh->vertexData->vertexBufferBinding->getBufferCount() > 0);
            virtBuffer = submesh->vertexData->vertexBufferBinding->getBuffer(0);
        }
        
        unsigned char* pVert = static_cast<unsigned char*>(virtBuffer->lock(HardwareBuffer::HBL_READ_ONLY));

        // need to lock the buffer since vertex data resides on GPU
        // and we need synchronization
        unsigned short* sindices = NULL;
        unsigned long* lindices = NULL;
        
        if (idxBuffer->getType() == HardwareIndexBuffer::IT_16BIT) {
            sindices = static_cast<unsigned short*>(idxBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
        } else if (idxBuffer->getType() == HardwareIndexBuffer::IT_32BIT) {
            lindices = static_cast<unsigned long*>(idxBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
        } else {
            assert(true == false);
        }
        
        const VertexElement* elm = virtDecl->findElementBySemantic(VES_POSITION, 0);
        int offset = elm->getOffset();
        assert(elm->getType() == VET_FLOAT3);
        
        for (int k = idxStart; k < idxStart + nIdx; k += 3) {
            unsigned int indices[3];
            btVector3 vertices[3];
            
            if (idxBuffer->getType() == HardwareIndexBuffer::IT_16BIT) {
                for (int l = 0; l < 3; ++l) {
                    indices[l] = sindices[k + l];
                }
            } else {
                for (int l = 0; l < 3; ++l) {
                    indices[l] = lindices[k + l];
                }
            }
            
            for (int l = 0; l < 3; ++l) { // for each vertex
                Real* posVert = (Real*)(pVert + indices[l] * virtBuffer->getVertexSize() + offset);
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
    
    Vector3 pos = entity->getParentSceneNode()->getPosition();
    Quaternion orient = entity->getParentSceneNode()->getOrientation();
    Vector3 scale = entity->getParentSceneNode()->getScale();
    
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

	// SPARKS: add it the dynamic manager if needed
	pEventManager->getDynamicObjectManager()->addObject(entity->getParentSceneNode(), btObj);

	entity->setMaterialName("Examples/WaterStream");
}

//================================================//

//
// This method will take every entity in your scene and register it as
// a mesh in the btCollisionWorld. NOTE: Be sure to call this function after
// you've added your static entities (environment) to the scene manager but
// before you add your characters. 
// 
void registerAllEntitiesAsColliders(SceneManager* sceneMgr, btCollisionWorld* colWorld, EventManager* pEventManager)
{
    SceneManager::MovableObjectIterator 
        i = sceneMgr->getMovableObjectIterator("Entity");
        
    while (i.hasMoreElements()) {
        Entity* entity = static_cast<Entity*>(i.getNext());
		
        registerEntityAsCollider(entity, colWorld, pEventManager);
    }
}

//================================================//
