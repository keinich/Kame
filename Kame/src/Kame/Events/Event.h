#pragma once

#include <map>
#include <functional>
#include <queue>

namespace Kame {

  //#define HANDLER(T) std::function<bool(T&)>

  //struct  EventHandle {

  //public:

  //  friend class EventBase;

  //  UINT GetHandle() { return _Handle; }

  //protected: // Methods

  //  EventHandle(UINT handle, EventBase* owner) {
  //    _Handle = handle;
  //    _Owner = owner;
  //  }
  //protected: // Fields

  //  EventBase* _Owner;
  //  UINT _Handle;
  //};

  class EventBase {

  public:

    //friend struct EventHandle;

    template<typename T>
    using EventHandler = std::function<bool(T&)>;

  protected: // Methods
    EventBase() : _Handled(false) {};
    virtual void RemoveHandler(const char* handle) = 0;
    //inline EventHandle CreateEventHandle(UINT handle, EventBase* owner) { return EventHandle(handle, owner); }

  protected: // Fields
    bool _Handled;

  };

  template<class T>
  class Event : public EventBase {

    //friend struct EventHandle;

  public:

    Event() :
      _MaxHandle(0) {}

    virtual ~Event() {
      RemoveAllHandlers();
    }

    void AddHandler(const char* identifier, EventHandler<T> handler);
    void RemoveHandler(const char* identifier) override;
    void RemoveAllHandlers();
    void Raise(T args);

  protected:

    //std::vector<EventHandler<T>> _Handlers;
    std::map<const char*, EventHandler<T>> _Handlers;

    UINT _MaxHandle;
    std::queue<UINT> _FreeHandles;
  };

  int fgh123(int, int);

  template<class T>
  void Event<T>::AddHandler(const char* identifier, EventHandler<T> handler) {
    _Handlers.insert(std::map<const char*, EventHandler<T>>::value_type(identifier, handler));
  }

  template<class T>
  inline void Event<T>::RemoveHandler(const char* handle) {
    _Handlers.erase(handle);
    //_FreeHandles.push(handle.GetHandle());
  }

  template<class T>
  inline void Event<T>::RemoveAllHandlers() {
    _Handlers.clear();
  }

  template<class T>
  inline void Event<T>::Raise(T args) {
    std::map<const char*, EventHandler<T>>::iterator it = _Handlers.begin();
    while (it != _Handlers.end()) {
      _Handled = it->second(*(T*)&args);
      it++;
    }
  }

}