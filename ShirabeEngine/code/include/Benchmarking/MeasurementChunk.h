#ifndef __SHIRABE__MEASUREMENT_CHUNK_H__
#define __SHIRABE__MEASUREMENT_CHUNK_H__

#include <iostream>
#include <wchar.h>

namespace Engine {
	namespace Benchmarking {
		template <typename ParameterT, typename ValueT>
		class MeasurementChunk {
		public:
			typedef ParameterT param_type;
			typedef ValueT     value_type;

			inline explicit MeasurementChunk(const param_type param = 0, const value_type value = 0) {
				_parameter = param;
				_value = value;
			}

			template <typename ParamU, typename ValueU>
			inline explicit MeasurementChunk(MeasurementChunk<ParamU, ValueU> const& instToCpy) {
				_parameter = instToCpy.parameter();
				_value = instToCpy.value();
			}

			inline ~MeasurementChunk() {}

		public:
			/* Return a const reference to the contained parameter to protect it
			 * from being changed without having to copy it. */
			virtual const param_type& parameter() const { return m_parameter; }
			/* Return a const reference to the contained value to protect it
			 * from being changed without having to copy it */
			virtual const value_type&     value()     const { return m_value; }

		protected:
			param_type m_parameter;
			value_type     m_value;

			/* Stream operators for printing to console or basic stream derivates. */
			friend std::basic_ostream<char> operator<<(std::basic_ostream<char>&                       strm,
													   const MeasurementChunk<param_type, value_type>& chunk)
			{
				return (strm << chunk.parameter() << ": " << chunk.value());
			}
			friend std::basic_ostream<wchar_t> operator<<(std::basic_ostream<wchar_t>&                    strm,
														  const MeasurementChunk<param_type, value_type>& chunk)
			{
				return (strm << chunk.parameter() << L": " << chunk.value());
			}
		};
	}
}

#endif