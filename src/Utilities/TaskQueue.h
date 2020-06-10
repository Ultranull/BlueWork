#pragma once

#include <deque>
#include <functional>
#include <mutex>
#include <memory>

template<typename ArgType>
struct Task {
	typedef std::function<void(ArgType)> Handle;

	Handle Notify;
	ArgType Data;

	void Execute() {
		Notify(Data);
	}

	template<typename Class>
	static Handle BindTask(void (Class::* f)(ArgType), Class* source) {
		return std::bind(f, static_cast<Class*>(source), std::placeholders::_1);
	}
};

template<typename ArgType>
class TaskQueue {
	std::mutex Exclusion;

	std::deque<std::unique_ptr<Task<ArgType>>> Events;

public:

	void Queue(Task<ArgType>* task) {
		Events.push_back(std::unique_ptr<Task<ArgType>>(task));
	}

	bool Contains(ArgType data) {
		for (int i = 0; i < Events.size();i++) {
			if (data == Events[i]->Data) {
				return true;
			}
		}
		return false;
	}

	int NumberOfTasks() {
		Exclusion.lock();
		int n = Events.size();
		Exclusion.unlock();
		return n;
	}

	ArgType GetNextTaskData() {
		if (Events.empty())
			return ArgType();
		return Events.front()->Data;
	}

	void ProcessNext() {
		Exclusion.lock();
		Task<ArgType>* task = Events.front().get();
		Exclusion.unlock();

		task->Execute();

		Exclusion.lock();
		Events.pop_front();
		Exclusion.unlock();
	}

	void Process() {
		while (!Events.empty()) {
			ProcessNext();
		}
	}
};
