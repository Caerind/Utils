namespace event
{
	template <typename T>
	bool VEmitter::_dispatch(const T& event) const
	{
		bool res = _checkFamily(T::family());
		assert(res);

		if(res)
		{
			for(Handler* handler: _handlers)
			{
				handler->_receive(*this,event);
			}
		}
		return res;
	}

	template <typename T>
	bool VEmitter::connect(Handler& handler,const std::function<void(const T&)>& callback)
	{
		bool res = _checkFamily(T::family());
		assert(res);
		
		if(res)
		{
			handler.connect<T>(*this,callback);
		}

		return res;

	}


}
