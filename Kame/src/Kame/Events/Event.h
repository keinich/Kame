#pragma once

#include <map>
//#include <functional>
#include "Kame/Core.h"

namespace Kame {

  //#define HANDLER(T) std::function<bool(T&)>

  struct  EventHandle {

  public:

    friend class EventBase;

    UINT GetHandle() { return _Handle; }

  protected: // Methods

    EventHandle(UINT handle, EventBase* owner) {
      _Handle = handle;
      _Owner = owner;
    }
  protected: // Fields

    EventBase* _Owner;
    UINT _Handle;
  };

  class EventBase {

  public:

    friend struct EventHandle;

    template<typename T>
    using EventHandler = std::function<bool(T&)>;

  protected: // Methods
    EventBase() : _Handled(false) {};
    virtual void RemoveHandler(EventHandle handle) = 0;
    inline EventHandle CreateEventHandle(UINT handle, EventBase* owner) { return EventHandle(handle, owner); }

  protected: // Fields
    bool _Handled;

  };

  template<class T>
  class Event : public EventBase {

    friend struct EventHandle;

  public:

    Event() :
      _MaxHandle(0) {}

    virtual ~Event() {
      RemoveAllHandlers();
    }

    EventHandle AddHandler(EventHandler<T> handler);
    void RemoveHandler(EventHandle handle) override;
    void RemoveAllHandlers();
    void Raise(T args);

  protected:

    //std::vector<EventHandler<T>> _Handlers;
    std::map<UINT, EventHandler<T>> _Handlers;

    UINT _MaxHandle;
    std::queue<UINT> _FreeHandles;
  };

  template<class T>
  EventHandle Event<T>::AddHandler(EventHandler<T> handler) {
    UINT uniqueIdentifier;
    if (_FreeHandles.empty()) {
      uniqueIdentifier = _MaxHandle;
      _MaxHandle++;
    }
    else {
      uniqueIdentifier = _FreeHandles.back();
      _FreeHandles.pop();
    }
    _Handlers.insert(std::map<UINT, EventHandler<T>>::value_type(uniqueIdentifier, handler));
    return CreateEventHandle(uniqueIdentifier, this);
  }

  template<class T>
  inline void Event<T>::RemoveHandler(EventHandle handle) {
    _Handlers.erase(handle.GetHandle());
    _FreeHandles.push(handle.GetHandle());
  }

  template<class T>
  inline void Event<T>::RemoveAllHandlers() {
    _Handlers.clear();
    _MaxHandle = 0;
    std::queue<UINT> empty;
    std::swap(_FreeHandles, empty);
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