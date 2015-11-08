namespace event
{
	/////////////////////// Helper /////////////////////

	template<typename T>
	inline void _helper_isEvent()
	{
		static_assert(std::is_base_of<Event<T>,T>::value, "Emmiter<Args ...> : Each Arg must be a class derived from Event<Arg>");
	}

	template<typename T, typename U, typename ... Args>
	inline void _helper_isEvent()
	{
		_helper_isEvent<T>();
		_helper_isEvent<U,Args ...>();
	}

	template<typename T>
	inline bool _helper_checkFamily(unsigned int family)
	{
		return T::family() == family;
	}

	template<typename T, typename U, typename ... Args>
	inline bool _helper_checkFamily(unsigned int family)
	{
		return _helper_checkFamily<T>(family) or _helper_checkFamily<U,Args ...>(family);
	}

	///////////// Emmiter ///////////////
	template <typename ... Args>
	Emitter<Args ...>::Emitter()
	{
		_helper_isEvent<Args ...>();
	}

	template <typename ... Args>
	Emitter<Args ...>::~Emitter()
	{
	}

	///////////////// PRIVATE ///////////////////
	
	template <typename ... Args>
	bool Emitter<Args ...>::_checkFamily(unsigned int family)const
	{
		return _helper_checkFamily<Args ...> (family);
	}

}
