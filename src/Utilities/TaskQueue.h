#pragma once

#include <vector>
#include <functional>
#include <mutex>
#include <memory>

struct Task {
	typedef std::function<void(const void*, int)> Handle;

	Handle Notify;
	void* Data;
	int DataSize;

	~Task();

	void Execute();

	template<typename Class>
	static Handle BindTask(void (Class::* f)(const void*, int), Class* source) {
		return std::bind(f, static_cast<Class*>(source), std::placeholders::_1, std::placeholders::_2);
	}
};

class TaskQueue {
	std::mutex Exclusion;

	std::vector<std::unique_ptr<Task>> Events;

public:

	void Queue(Task* task);

	int NumberOfTasks();

	void ProcessNext();

	void Process();
};
