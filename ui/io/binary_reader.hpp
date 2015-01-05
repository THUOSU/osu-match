/*!
* THUOSU Library Defination
* 
* \brief binary reader for osu associated files
*
*/

#ifndef THUOSU_BINARY_READER_HPP
#define THUOSU_BINARY_READER_HPP

#include <memory>
#include <string>
#include <iostream>
#include <utility>
#include <cstdint>
#include <locale>
#include <codecvt>
#include <stdexcept>
#include <type_traits>

namespace thuosu
{
	namespace io
	{
		//! \brief utf-8 string <-> wstring converter
		static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_w_converter{};

		template <typename InStream>
		struct binary_reader final
		{
			binary_reader(InStream & in) : m_pIn{ &in } {}

			template <typename T>
			typename std::enable_if<std::is_arithmetic<T>::value, binary_reader &>::type operator >> (T & value)
			{
				m_pIn->read(reinterpret_cast<char*>(&value), sizeof(T));
				return *this;
			}

			binary_reader & operator >> (std::wstring & value)
			{
				std::int8_t num;
				(*this) >> num;
				if (num == 0)
					value = L"";
				else if (num == 0x0B)
				{
					int length = read_7bit_encoded_int();
					if (length < 0)
						throw std::range_error("invalid string length");
					if (length == 0)
						value = L"";
					else
					{
						if (length > m_buf_size)
						{
							m_buf_size *= 2;
							m_buf.reset(new char[m_buf_size]);
						}
						m_pIn->read(m_buf.get(), length);
						value = utf8_w_converter.from_bytes(m_buf.get(), m_buf.get() + length);
					}
				}
				return *this;
			}

			binary_reader & operator >> (std::string & value)
			{
				std::int8_t num;
				*this >> num;
				if (num == 0)
					value = "";
				else if (num == 0x0B)
				{
					int length = read_7bit_encoded_int();
					if (length < 0)
						throw std::range_error("invalid string length");
					if (length == 0)
						value = "";
					else
					{
						if (length > m_buf_size)
						{
							m_buf_size *= 2;
							m_buf.reset(new char[m_buf_size]);
						}
						m_pIn->read(m_buf.get(), length);
						value = std::string{ m_buf.get(), m_buf.get() + length };
					}
				}
				return *this;
			}

			template <typename T>
			T read()
			{
				T value;
				*this >> value;
				return value;
			}

			void ignore(int length)
			{
				m_pIn->ignore(length);
			}

			void ignore_string()
			{
				std::int8_t num;
				*this >> num;
				if (num == 0)
					return;
				if (num == 0x0B)
				{
					int length = read_7bit_encoded_int();
					if (length < 0)
						throw std::range_error("invalid string length");
					if (length == 0)
						return;
					ignore(length);
				}
			}

			template <typename T>
			void ignore()
			{
				ignorer<T>::ignore(*this);
			}

		private:

			template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value, void>>
			struct ignorer
			{
				inline static void ignore(binary_reader& reader) { reader.ignore(sizeof(T)); }
			};

			template <>
			struct ignorer < std::wstring >
			{
				inline static void ignore(binary_reader& reader) { reader.ignore_string(); }
			};

			template <>
			struct ignorer < std::string >
			{
				inline static void ignore(binary_reader& reader) { reader.ignore_string(); }
			};

			int read_7bit_encoded_int()
			{
				int num = 0;
				int num2 = 0;
				std::int8_t b;
				while (num2 != 35)
				{
					*this >> b;
					num |= (int)(b & 127) << num2;
					num2 += 7;
					if ((b & 128) == 0)
						return num;
				}
				throw std::range_error("invalid 7bit encoded length");
			}

		private:

			InStream * m_pIn;
			std::unique_ptr<char[]> m_buf = new char[256];
			int m_buf_size = 256;
		};

		template <typename InStream>
		inline binary_reader<InStream> make_binary_reader(InStream & in)
		{
			return binary_reader<InStream>(in);
		}
	}
}


#endif // THUOSU_BINARY_READER_HPP
