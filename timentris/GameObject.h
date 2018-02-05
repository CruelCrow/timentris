#pragma once
#include <atomic>
#include <mutex>

using namespace std;

class GameObject {
private:
	mutex _mutex;
public:
	GameObject();
	~GameObject();
	GameObject(const GameObject& other);
	GameObject& operator=(const GameObject& other);

	mutex& getMutex();
	void lock();
	void unlock();
};

