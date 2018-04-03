// CppMeetup.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <memory>
#include <stdint.h>

template <typename T>
using Ptr = std::shared_ptr<T>;

template <typename T, typename...Args>
static Ptr<T> MakePtr(Args...args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename TType>
class ITaskBackendPartDecl {
public:
  virtual uint8_t spawnCreationTask(typename TType::Descriptor const&) = 0;
};

//template <typename... TTypes>
//class AbstractTaskBackend
//  : public ITaskBackendPartDecl<TTypes>...
//{ };
//
template <typename... TTypes>
class AbstractTaskBackend
  : public ITaskBackendPartDecl<TTypes>...
{
public:
  template <typename TType>
  uint8_t spawnCreationTask(typename TType::Descriptor const&desc) {
    // How to select the proper override?
  }
};

template <typename... TTypes>
class AbstractTaskBackend
  : public ITaskBackendPartDecl<TTypes>...
{
public:
  template <typename TType>
  uint8_t spawnCreationTask(typename TType::Descriptor const&desc) {
    // Try simple overload-resolution using the descriptor. 
    // Fails, as TType is part of the nested-name-specifier and 
    // TType::Descriptor is non-deduced scope 
    // -> Deduction fails 
    // -> Overload resolution fails
    return this->spawnCreationTask(desc);
  }

  template <typename TType>
  uint8_t spawnCreationTask(typename TType::Descriptor const&desc) {
    // Qualified Name Lookup in explicitly defined scope of ITaskBackendPartDecl<TType>.
    // Qualified name look up is never virtual.
    // Just finds a pure-virtual method --> Undefined Reference
    return this->ITaskBackendPartDecl<TType>::spawnCreationTask(desc);
  }

  template <typename TType>
  uint8_t spawnCreationTask(typename TType::Descriptor const&desc) {
    // Type cast to desired interface and perform virtual function call
    // -> Selects the proper override
    return static_cast<ITaskBackendPartDecl<TType>*>(this)->spawnCreationTask(desc);
  }
};

struct TypeA { struct Descriptor {}; };
struct TypeB { struct Descriptor {}; };
struct TypeC { struct Descriptor {}; };

#define SupportedTypes TypeA, TypeB, TypeC

class ExampleTaskBackend
  : public AbstractTaskBackend<SupportedTypes>
{
private:
  uint8_t spawnCreationTask(TypeA::Descriptor const&);
  uint8_t spawnCreationTask(TypeB::Descriptor const&);
  uint8_t spawnCreationTask(TypeC::Descriptor const&);
};

uint8_t ExampleTaskBackend::spawnCreationTask(TypeA::Descriptor const&)
{
  return 1;
}

uint8_t ExampleTaskBackend::spawnCreationTask(TypeB::Descriptor const&)
{
  return 2;
}

uint8_t ExampleTaskBackend::spawnCreationTask(TypeC::Descriptor const&)
{
  return 4;
}

void print(uint8_t value) {
  std::cout << "Value: " << (uint16_t)value << std::endl;
}

int main()
{
  Ptr<AbstractTaskBackend<SupportedTypes>> backend = MakePtr<ExampleTaskBackend>();

  TypeA::Descriptor descA={ };
  print(backend->spawnCreationTask<TypeA>(descA));
  TypeB::Descriptor descB={ };
  print(backend->spawnCreationTask<TypeB>(descB));
  TypeC::Descriptor descC={ };
  print(backend->spawnCreationTask<TypeC>(descC));

  system("PAUSE");
}

class IInterface {
public:
  IInterface(IInterface const&other)            = delete;
  IInterface(IInterface &&other)                = delete;
  IInterface& operator=(IInterface const&other) = delete;
  IInterface& operator=(IInterface &&other)     = delete;

  virtual ~IInterface() = default;

  // Public API
  virtual void foo() = 0;

protected:
  IInterface() = default;
};