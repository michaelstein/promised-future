#pragma once
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>
#include <vector>

namespace pf {

enum class State : uint8_t
{
	PENDING,
	RESOLVED,
	REJECTED,
	INVALID
};

class AbstractPromise
{
public:
	virtual ~AbstractPromise() = default;
	virtual State state() const = 0;
};

using Promises = std::vector<std::shared_ptr<AbstractPromise>>;

template<class T, class E = uint8_t> requires (!std::is_abstract_v<T>)
class Promise : public AbstractPromise
{
public:
	using This = Promise<T, E>;
	using ValueType = T;
	using ErrorType = E;
	using ResolveFunction = std::function<void(const T&)>;
	using RejectFunction = std::function<void(const E&)>;

	Promise(const This& other) = delete;
	Promise(This&& other) = default;

	This& operator=(const This& other) = delete;
	This& operator=(This&& other) = default;

	Promise(std::function<void(ResolveFunction, RejectFunction)> asyncFunc)
	{
		ResolveFunction resolveFunc = std::bind(&This::resolveInternal, this, std::placeholders::_1);
		RejectFunction rejectFunc = std::bind(&This::rejectInternal, this, std::placeholders::_1);
		_future = std::async(std::launch::async, asyncFunc, resolveFunc, rejectFunc);
	}

	State state() const override
	{
		std::lock_guard lock(_mutex);
		if (_value)
			return State::RESOLVED;
		if (_error)
			return State::REJECTED;
		if (_future.valid())
			return State::PENDING;
		return State::INVALID;
	}

	template<class ResultT, class ResultE = E> requires (!std::is_void_v<ResultT>)
	std::shared_ptr<Promise<ResultT, ResultE>> then(std::function<ResultT(const T&)> onResolved, RejectFunction onRejected = {})
	{
		auto& future = _future;
		const auto& value = _value;
		const auto& error = _error;

		auto ret = std::make_shared<Promise<ResultT, ResultE>>([future, &value, &error, onResolved, onRejected](auto resolve, auto reject) {
			// Do not need to lock, because future is safe. After get() async() has finished.
			if (future.valid())
				future.get();

			if (value && onResolved)
				resolve(onResolved(value.value()));

			if (error && onRejected)
				onRejected(error.value());
		});

		_chain.push_back(ret);
		return ret;
	}

	template<class ResultT, class ResultE = E> requires std::is_void_v<ResultT>
	std::shared_ptr<Promise<uint8_t, ResultE>> then(std::function<ResultT(const T&)> onResolved, RejectFunction onRejected = {})
	{
		auto& future = _future;
		const auto& value = _value;
		const auto& error = _error;

		auto ret = std::make_shared<Promise<uint8_t, ResultE>>([future, &value, &error, onResolved, onRejected](auto resolve, auto reject) {
			// Do not need to lock, because future is safe. After get() async() has finished.
			if (future.valid())
				future.get();

			if (value && onResolved)
				onResolved(value.value());

			if (error && onRejected)
				onRejected(error.value());

			resolve(0);
		});

		_chain.push_back(ret);
		return ret;
	}

	void finally(std::function<void(const T&)> onResolved, RejectFunction onRejected = {})
	{
		then<void>(onResolved, onRejected);
	}

	void wait()
	{
		if (_future.valid())
			_future.wait();
	}

	std::optional<T> value() const
	{
		std::lock_guard lock(_mutex);
		return _value;
	}

	const std::optional<T>& valueRef() const
	{
		return _value;
	}

	static This resolve(const T& value)
	{
		return This(value, {});
	}

	static This reject(const E& error)
	{
		return This({}, error);
	}

protected:
	Promise(const std::optional<T>& value, const std::optional<E>& error)
		: _value(value)
		, _error(error)
	{
	}

	void resolveInternal(const T& value)
	{
		std::lock_guard lock(_mutex);
		_value = value;
	}

	void rejectInternal(const E& error)
	{
		std::lock_guard lock(_mutex);
		_error = error;
	}

	std::optional<T> _value;
	std::optional<E> _error;
	std::shared_future<void> _future;
	mutable std::mutex _mutex;
	Promises _chain;
};

template<class T, class E = uint8_t>
using SharedPromise = std::shared_ptr<Promise<T, E>>;

template<class T, class E = uint8_t>
inline SharedPromise<T, E> promise(std::function<void(typename Promise<T, E>::ResolveFunction, typename Promise<T, E>::RejectFunction)> asyncFunc)
{
	return std::make_shared<Promise<T, E>>(asyncFunc);
}

template<class E = uint8_t, class T>
inline SharedPromise<T, E> resolve(const T& value)
{
	return std::make_shared<Promise<T, E>>(Promise<T, E>::resolve(value));
}

template<class T, class E>
inline SharedPromise<T, E> reject(const E& error)
{
	return std::make_shared<Promise<T, E>>(Promise<T, E>::reject(error));
}

Promises::size_type clear(Promises& promises)
{
	Promises::size_type ret = 0;
	auto iter = promises.cbegin();
	while (iter != promises.cend())
	{
		const auto ptr = *iter;
		const auto state = ptr ? ptr->state() : State::INVALID;
		if (state != State::PENDING)
		{
			iter = promises.erase(iter);
			++ret;
		}
	}
	return ret;
}

}