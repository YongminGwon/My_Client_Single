#pragma once
#include "Entity.h"
#include "iostream"
#include <algorithm>

using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
	EntityVec                                  entities;
	EntityVec                                  entitiesToAdd;
	std::map<std::string, EntityVec>           entityMap;
	size_t                                     totalEntities = 0;

	void removeDeadEntities(EntityVec& vec)
	{
		//TODO: remove all dead entities from the input vector
		// this is called by the update() function
		vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& entity) {return !entity->isActive(); }), vec.end());
	}
public:
	EntityManager() = default;

	void update()
	{
		//TODO: add entities from the entitiesToAdd to the proper location(s)
		// -add them to the vector of all entities
		// -add them to the vector inside the map, with the tag as a key
		// remove dead entities from the vector of all entities
		for (auto& e : entitiesToAdd)
		{
			entities.push_back(e);
			entityMap[e->getTag()].push_back(e);
		}
		entitiesToAdd.clear();
		removeDeadEntities(entities);
		//remove dead entities from each vector in the entity map
		//C++20 way of iterating through [key, value] pairs in a map
		for (auto& [tag, entityVec] : entityMap)
		{
			removeDeadEntities(entityVec);
		}
	}

	std::shared_ptr<Entity> addEntity(const std::string& tag)
	{
		//create the entity shared pointer
		auto entity = std::shared_ptr<Entity>(new Entity(totalEntities++, tag));

		// add it to the vec of all entities
		entitiesToAdd.push_back(entity);

		//add it to the entity map
		if (entityMap.find(tag) == entityMap.end()) { entityMap[tag] = EntityVec(); }
		entityMap[tag].push_back(entity);

		return entity;
	}

	const EntityVec& getEntities()
	{
		return entities;
	}

	const EntityVec& getEntities(const std::string& tag)
	{
		if (entityMap.find(tag) == entityMap.end()) { entityMap[tag] = EntityVec(); }
		return entityMap[tag];
	}

	const std::map<std::string, EntityVec>& getEntityMap()
	{
		return entityMap;
	}
};