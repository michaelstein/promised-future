#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace pf {

enum class State : uint8_t
{
	PENDING,
	RESOLVED,
	REJECTED
};

template<class T, class E> requires (!std::is_abstract_v<T>)
class GenericPromise
{
public:
	using This = GenericPromise<T, E>;
	using ValueType = T;
	using ErrorType = E;
	using ResolveFunction = std::function<void(const T&)>;
	using RejectFunction = std::function<void(const E&)>;

	GenericPromise(std::function<void(ResolveFunction, RejectFunction)> asyncFunc)
	{
		ResolveFunction resolveFunc = std::bind(&This::resolveInternal, this, std::placeholders::_1);
		RejectFunction rejectFunc = std::bind(&This::rejectInternal, this, std::placeholders::_1);
		_future = std::async(std::launch::async, asyncFunc, resolveFunc, rejectFunc);
	}

	virtual ~GenericPromise() = default;

	template<class ResultT, class ResultE = E> requires (!std::is_void_v<ResultT>)
	std::shared_ptr<GenericPromise<ResultT, ResultE>> then(std::function<ResultT(const T&)> onResolved, RejectFunction onRejected = {})
	{
		auto& future = _future;
		const auto& value = _value;
		const auto& error = _error;

		auto ret = std::make_shared<GenericPromise<ResultT, ResultE>>([future, &value, &error, onResolved, onRejected](auto resolve, auto reject) {
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
	std::shared_ptr<GenericPromise<uint8_t, ResultE>> then(std::function<ResultT(const T&)> onResolved, RejectFunction onRejected = {})
	{
		auto& future = _future;
		const auto& value = _value;
		const auto& error = _error;

		auto ret = std::make_shared<GenericPromise<uint8_t, ResultE>>([future, &value, &error, onResolved, onRejected](auto resolve, auto reject) {
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

	static This resolve(const T& value)
	{
		return This(value, {});
	}

	static This reject(const E& error)
	{
		return This({}, error);
	}

protected:
	GenericPromise(const std::optional<T>& value, const std::optional<E>& error)
		: _value(value)
		, _error(error)
	{
	}

	void resolveInternal(const T& value)
	{
		_value = value;
	}

	void rejectInternal(const E& error)
	{
		_error = error;
	}

	std::optional<T> _value;
	std::optional<E> _error;
	std::shared_future<void> _future;
	std::vector<std::shared_ptr<void>> _chain;
};

template<class T>
using Promise = GenericPromise<T, uint8_t>;

template<class E = uint8_t, class T>
GenericPromise<T, E> resolve(const T& value)
{
	return GenericPromise<T, E>::resolve(value);
}

}