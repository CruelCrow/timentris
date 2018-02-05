#include "GameObject.h"

GameObject::GameObject() {
}
GameObject::~GameObject() {
}
GameObject::GameObject(const GameObject& other) {
	if (this == &other) {
		return;
	}
}
GameObject& GameObject::operator=(const GameObject& other) {
	return *this;
}


mutex& GameObject::getMutex() {
	return _mutex;
}

void GameObject::lock() {
	getMutex().lock();
}

void GameObject::unlock() {
	getMutex().unlock();
}