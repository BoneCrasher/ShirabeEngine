#ifndef __SHIRABE_DESIGN_PASSKEY_H__
#define __SHIRABE_DESIGN_PASSKEY_H__

namespace engine {

  template <typename TAccessor>
  class PassKey {
  private:
    friend TAccessor;
    PassKey()                          = default;
    PassKey(PassKey const&)            = default;
    PassKey& operator=(PassKey const&) = delete;
  };

}

#endif