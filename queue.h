#include <deque>

template<typename T>
class tsqueue
{
public:
	tsqueue() = default;
	virtual ~tsqueue()
	{
		this->Clear();
	}
public:
	//returns the item at the front of the queue
	const T& Front()
	{
		return this->deqQueue.front();
	}

	// returns the item at the back of the queue
	const T& Back()
	{
		return this->deqQueue.back();
	}

	void PushBack(const T& item)
	{
		this->deqQueue.emplace_back(std::move(item));
	}

	void PushFront(const T& item)
	{
		this->deqQueue.emplace_front(std::move(item));
	}

	bool Empty()
	{
		return this->deqQueue.empty();
	}

	size_t Count()
	{
		return this->deqQueue.size();
	}

	void Clear()
	{
		this->deqQueue.clear();
	}

	T PopFront()
	{
		T t = std::move(this->deqQueue.front());
		this->deqQueue.pop_front();
		return t;
	}

	T PopBack()
	{
		T t = std::move(this->deqQueue.back());
		this->deqQueue.pop_back();
		return t;
	}
protected:
	std::deque<T> deqQueue;
};