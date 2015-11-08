#include "VEmitter.hpp"

namespace event
{
    template<typename T>
    Event<T>::Event()
    {
    }

    template<typename T>
    Event<T>::~Event()
    {
    }

    //////////////// PRIVATE /////////////////

    template<typename T>
    unsigned int Event<T>::family()
    {
        static unsigned int family = VEvent::_familyCounter++;
        return family;
    };

    template<typename T>
    bool Event<T>::_dispatch(const VEmitter& emitter) const
    {
        return emitter._dispatch(*this);
    }

}
