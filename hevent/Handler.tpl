namespace event
{

	template <typename T>
	void Handler::bind(const std::function<void(const T&)>& callback)
	{
		_callbacks[T::family()].reset(new Function<T>(callback));
	}

	template <typename T>
	void Handler::connect(VEmitter& emitter,const std::function<void(const T&)>& callback)
	{
		connect(emitter);
		_callbacksByEmitter[&emitter][T::family()].reset(new Function<T>(callback));
	}
	
	//////////// PROTECTED /////////////////

	template <typename T>
	Handler::Function<T>::Function(const std::function<void(const T&)>& callback) : _callback(callback)
	{
	}

	template <typename T>
	Handler::Function<T>::~Function()
	{
	}

	template <typename T>
	void Handler::Function<T>::exec(const VEvent& event)const
	{
		_callback(static_cast<const T&>(event));
	}

	//////////// PRIVATE /////////////////

	template <typename T>
	void Handler::_receive(const VEmitter& emitter,const T& event)const
	{
		{
			auto it = _callbacksByEmitter.find(const_cast<VEmitter*>(&emitter));
			if(it != _callbacksByEmitter.end())
			{
				auto it2 = it->second.find(T::family());
				if(it2 != it->second.end())
				{
					it2->second->exec(event);
					return;
				}
				else if(_defaultCallback)
				{
					_defaultCallback(event);
					return;
				}
			}
		}

		{
			auto it = _callbacks.find(T::family());
			if(it != _callbacks.end())
			{
				it->second->exec(event);
				return;
			}
		}

		if(_defaultCallback)
			_defaultCallback(event);
	}
}
