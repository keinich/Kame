#pragma once

#include <map>
//#include <functional>
#include "Kame/Core.h"

namespace Kame {

  //#define HANDLER(T) std::function<bool(T&)>

  class EventBase {

    template<typename T>
    using EventHandler = std::function<bool(T&)>;

  protected: // Methods
    EventBase() : _Handled(false) {};

  protected: // Fields

    bool _Handled;

  };

  template<class T>
  class Event : public EventBase {

  public:

    Event() {}

    void AddHandler(UINT uniqueIdentifier, EventHandler<T> handler);
    void RemoveHandler(UINT uniqueIdentifier);
    void Raise(T args);

  protected:

    //std::vector<EventHandler<T>> _Handlers;
    std::map<UINT, EventHandler<T>> _Handlers;

  };

  template<class T>
  inline void Event<T>::AddHandler(UINT uniqueIdentifier, EventHandler<T> handler) {
    _Handlers.insert(std::map<UINT, EventHandler<T>>::value_type(uniqueIdentifier, handler));
  }

  template<class T>
  inline void Event<T>::RemoveHandler(UINT uniqueIdentifier) {
    _Handlers.erase(uniqueIdentifier);
    //_Handlers.find(uniqueIdentifier);
    //std::vector<EventHandler<T>>::iterator it = std::find(_Handlers.begin(), _Handlers.end(), handler);
    /*   if (it != _Handlers.end()) {
         _Handlers.erase(it);
       }*/
  }

  template<class T>
  inline void Event<T>::Raise(T args) {
    std::map<UINT, EventHandler<T>>::iterator it = _Handlers.begin();
    while (it != _Handlers.end()) {
      _Handled = it->second(*(T*)&args);
      it++;
    }
  }

}