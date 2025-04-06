#pragma once

#include "Component.h"
#include <vector>
#include <memory>
#include <typeindex>
#include <map>

class EntityManager;

using ComponentTuple = std::tuple<Transform, Shape, Collision, Score, Input, Lifespan>;

class Entity
{
	friend class EntityManager;

	ComponentTuple components;
	bool           active = true;
	std::string    tag = "default";
	size_t         id = 0;

	Entity(const size_t& id, const std::string& tag) : tag(tag), id(id)
	{

	}

public:
	bool isActive() const {return active;}
	void destroy() {active = false;}
	size_t getId()const{return id;}
	const std::string& getTag() const {return tag;}

	template<typename T>
	bool has() const
	{
		return get<T>().exists;
	}

	template<typename T, typename... TArgs>
	T& add(TArgs&&... Args)
	{
		auto& component = get<T>();
		component = T( std::forward<TArgs>(Args)... );
		component.exists = true;
		return component;
	}

	template<typename T>
	T& get()
	{
		return std::get<T>(components);
	}

	template<typename T>
	const T& get() const
	{
		return std::get<T>(components);
	}

	template<typename T>
	void remove()
	{
		get<T>() = T();
	}
};