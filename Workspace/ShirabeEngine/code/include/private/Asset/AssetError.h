#include <exception>
#include <string>

namespace Engine {
  namespace Asset {

    enum class AssetErrorCode {
      Ok,
      AssetNotFound,
      AssetAlreadyAdded,
    };

    class AssetError
      : public std::runtime_error
    {
    public:
      inline
      AssetError(
        std::string    const&msg,
        AssetErrorCode const&code)
        : std::runtime_error(msg)
        , m_code(code)
      {}

      inline AssetErrorCode code() const { return m_code; }

    private:
      AssetErrorCode m_code;
    };

  }
}