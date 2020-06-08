#include "TaskQueue.h"

Task::~Task() {
	delete Data;
}

void Task::Execute() {
	Notify(Data, DataSize);
}

void TaskQueue::Queue(Task* task) {
	Events.push_back(std::unique_ptr<Task>(task));
}

int TaskQueue::NumberOfTasks() {
	 Exclusion.lock();
	 int n = Events.size();
	 Exclusion.unlock();
	 return n;
 }

void TaskQueue::ProcessNext() {
	 Exclusion.lock();
	 Task* task = Events.back().get();
	 Exclusion.unlock();

	 task->Execute();

	 Exclusion.lock();
	 Events.pop_back();
	 Exclusion.unlock();
 }

void TaskQueue::Process() {
	 while (!Events.empty()) {
		 ProcessNext();
	 }
 }
