/*!
* THUOSU Library Defination
* 
* \brief beatmap information and general-purpose osu!.db reading
*  
*/

#ifndef THUOSU_BEATMAP_INFO_HPP
#define THUOSU_BEATMAP_INFO_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "binary_reader.hpp"

#include "type_traits.hpp"

//!\brief default get member
//! enabled if T has member of type '_type' named '_member'
//! call reader.operator >> (_type &)
#define THUOSU_DEFAULT_GET_MEMBER(_member, _type) \
	template <typename Reader, typename T> \
	inline typename std::enable_if<detail::has_##_member<T, _type>::value, void>::type \
		get_##_member(Reader & reader, T & t) { reader >> t._member; } \

//!\brief default ignore member
//! call reader.ignore<_type>()
#define THUOSU_DEFAULT_IGNORE_MEMBER(_member, _type) \
	template <typename Reader, typename ...T> \
	inline void get_##_member(Reader & reader, const T & ...) { reader.ignore<_type>(); } \

#define THUOSU_DEFAULT_GET_STRING_MEMBER(_member) \
	namespace detail { THUOSU_CREATE_HAS_MEMBER(_member) } \
	THUOSU_DEFAULT_GET_MEMBER(_member, std::wstring) \
	THUOSU_DEFAULT_GET_MEMBER(_member, std::string) \
	THUOSU_DEFAULT_IGNORE_MEMBER(_member, std::string) \

#define THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, _type) \
	namespace detail { THUOSU_CREATE_HAS_MEMBER(_member) } \
	THUOSU_DEFAULT_GET_MEMBER(_member, _type) \
	THUOSU_DEFAULT_IGNORE_MEMBER(_member, _type) \

#define THUOSU_DEFAULT_GET_INT8_MEMBER(_member) THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, std::int8_t)
#define THUOSU_DEFAULT_GET_INT16_MEMBER(_member) THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, std::int16_t)
#define THUOSU_DEFAULT_GET_INT32_MEMBER(_member) THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, std::int32_t)
#define THUOSU_DEFAULT_GET_INT64_MEMBER(_member) THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, std::int64_t)
#define THUOSU_DEFAULT_GET_FLOAT32_MEMBER(_member) THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, float)
#define THUOSU_DEFAULT_GET_FLOAT64_MEMBER(_member) THUOSU_DEFAULT_GET_ARITHMETIC_MEMBER(_member, double)

namespace thuosu
{
	namespace io
	{
		//! \brief default get_xxx(ReaderType, InfoType) for general-purpose information types
		namespace accessor
		{
			//! \brief get_artist(ReaderType, InfoType)
			//! read string or wstring from reader if InfoType has member artist with empty value
			namespace detail { THUOSU_CREATE_HAS_MEMBER(artist) }
			template <typename Reader, typename T>
			inline typename std::enable_if<
				detail::has_artist<T, std::wstring>::value
				|| detail::has_artist<T, std::string>::value,
				void>::type
				get_artist(Reader & reader, T & t)
			{
				auto s = reader.read<decltype(t.artist)>();
				if (s.size() != 0)
					t.artist = std::move(s);
			}
			THUOSU_DEFAULT_IGNORE_MEMBER(artist, std::string)

			//! \brief get_title(ReaderType, InfoType)
			//! read string or wstring from reader if InfoType has member title with empty value
			namespace detail { THUOSU_CREATE_HAS_MEMBER(title) }
			template <typename Reader, typename T>
			inline typename std::enable_if<
				detail::has_title<T, std::wstring>::value
				|| detail::has_title<T, std::string>::value,
				void>::type
				get_title(Reader & reader, T & t)
			{
				auto s = reader.read<decltype(t.title)>();
				if (s.size() != 0)
					t.title = std::move(s);
			}
			THUOSU_DEFAULT_IGNORE_MEMBER(title, std::string)

			//! \brief get_artist_romanized(ReaderType, InfoType)
			//! read string or wstring from reader if InfoType has member artist_romanized or artist only
			namespace detail { THUOSU_CREATE_HAS_MEMBER(artist_romanized) }
			THUOSU_DEFAULT_GET_MEMBER(artist_romanized, std::string)
			THUOSU_DEFAULT_GET_MEMBER(artist_romanized, std::wstring)
			template <typename Reader, typename T>
			inline typename std::enable_if<
				!detail::has_artist_romanized<T, std::string>::value
				&& !detail::has_artist_romanized<T, std::wstring>::value
				&& (detail::has_artist<T, std::string>::value
				|| detail::has_artist<T, std::wstring>::value),
				void>::type
				get_artist_romanized(Reader & reader, T & t) { reader >> t.artist; }
			THUOSU_DEFAULT_IGNORE_MEMBER(artist_romanized, std::string)

			//! \brief get_title_romanized(ReaderType, InfoType)
			//! read string or wstring from reader if InfoType has member title_romanized or title only
			namespace detail { THUOSU_CREATE_HAS_MEMBER(title_romanized) }
			THUOSU_DEFAULT_GET_MEMBER(title_romanized, std::string)
			THUOSU_DEFAULT_GET_MEMBER(title_romanized, std::wstring)
			template <typename Reader, typename T>
			inline typename std::enable_if<
				!detail::has_title_romanized<T, std::string>::value
				&& !detail::has_title_romanized<T, std::wstring>::value
				&& (detail::has_title<T, std::string>::value
				|| detail::has_title<T, std::wstring>::value),
				void>::type
				get_title_romanized(Reader & reader, T & t) { reader >> t.title; }
			THUOSU_DEFAULT_IGNORE_MEMBER(title_romanized, std::string)

			//! other get_member()s
			THUOSU_DEFAULT_GET_STRING_MEMBER(creator)
			THUOSU_DEFAULT_GET_STRING_MEMBER(difficulty)
			THUOSU_DEFAULT_GET_STRING_MEMBER(music_file)
			THUOSU_DEFAULT_GET_STRING_MEMBER(beatmap_md5)
			THUOSU_DEFAULT_GET_STRING_MEMBER(beatmap_file)
			THUOSU_DEFAULT_GET_STRING_MEMBER(source)
			THUOSU_DEFAULT_GET_STRING_MEMBER(tags)
			THUOSU_DEFAULT_GET_STRING_MEMBER(location)
			THUOSU_DEFAULT_GET_STRING_MEMBER(title_with_font)
			
			THUOSU_DEFAULT_GET_INT8_MEMBER(rank_status)
			THUOSU_DEFAULT_GET_INT8_MEMBER(play_mode)
			
			THUOSU_DEFAULT_GET_INT16_MEMBER(circle_count)
			THUOSU_DEFAULT_GET_INT16_MEMBER(slider_count)
			THUOSU_DEFAULT_GET_INT16_MEMBER(spinner_count)
			THUOSU_DEFAULT_GET_INT16_MEMBER(online_offset)
			THUOSU_DEFAULT_GET_INT16_MEMBER(user_offset)
			
			THUOSU_DEFAULT_GET_INT32_MEMBER(drain_time)
			THUOSU_DEFAULT_GET_INT32_MEMBER(total_time)
			THUOSU_DEFAULT_GET_INT32_MEMBER(preview_time)
			THUOSU_DEFAULT_GET_INT32_MEMBER(beatmap_id)
			THUOSU_DEFAULT_GET_INT32_MEMBER(beatmap_set_id)
			THUOSU_DEFAULT_GET_INT32_MEMBER(beatmap_thread_id)

			THUOSU_DEFAULT_GET_INT64_MEMBER(last_edit_time)
			
			THUOSU_DEFAULT_GET_FLOAT32_MEMBER(ar)
			THUOSU_DEFAULT_GET_FLOAT32_MEMBER(cs)
			THUOSU_DEFAULT_GET_FLOAT32_MEMBER(hp)
			THUOSU_DEFAULT_GET_FLOAT32_MEMBER(od)
			THUOSU_DEFAULT_GET_FLOAT32_MEMBER(stack_leniency)

			THUOSU_DEFAULT_GET_FLOAT64_MEMBER(slider_velocity)

			// default assign romanized 

			template <typename T>
			inline typename std::enable_if<
				(detail::has_artist<T, std::wstring>::value && detail::has_artist_romanized<T, std::wstring>::value)
				|| (detail::has_artist<T, std::string>::value && detail::has_artist_romanized<T, std::string>::value),
				void>::type
				assign_artist(T & t) { if (t.artist.size() == 0) t.artist = t.artist_romanized; }
			template <typename T>
			inline typename std::enable_if<
				detail::has_artist<T, std::wstring>::value && detail::has_artist_romanized<T, std::string>::value,
				void>::type
				assign_artist(T & t) { if (t.artist.size() == 0) t.artist = utf8_w_converter.from_bytes(t.artist_romanized); }
			template <typename ...T>
			inline void assign_artist(const T & ... ) {}

			template <typename T>
			inline typename std::enable_if<
				(detail::has_title<T, std::wstring>::value && detail::has_title_romanized<T, std::wstring>::value)
				|| (detail::has_title<T, std::string>::value && detail::has_title_romanized<T, std::string>::value),
				void>::type
				assign_title(T & t) { if (t.title.size() == 0) t.title = t.title_romanized; }
			template <typename T>
			inline typename std::enable_if<
				detail::has_title<T, std::wstring>::value && detail::has_title_romanized<T, std::string>::value,
				void>::type
				assign_title(T & t) { if (t.title.size() == 0) t.title = utf8_w_converter.from_bytes(t.title_romanized); }
			template <typename ...T>
			inline void assign_title(const T & ...) {}
		}

		//! \brief beatmap info struct that contains all information in osu!.db
		struct beatmap_info
		{
			std::wstring
				artist,
				title,
				source,
				tags,
				title_with_font;
			std::string
				artist_romanized,
				title_romanized,
				creator,
				difficulty,
				music_file,
				beatmap_md5,
				beatmap_file,
				location;
			std::int8_t rank_status, play_mode;
			std::int16_t circle_count,
				slider_count,
				spinner_count,
				user_offset,
				online_offset;
			std::int32_t drain_time,
				total_time,
				preview_time,
				beatmap_id,
				beatmap_set_id,
				beatmap_thread_id;
			float ar, cs, hp, od, stack_leniency;
			double slider_velocity;
		};

		//! \brief load beatmap info from osu!.db
		//! load osu!.db beatmap list, compatible to general-purpose beatmap info struct
		template <typename InfoType, typename FilenameType>
		std::vector<InfoType> load_beatmap_info(const FilenameType & filename, bool assign_romanized = true)
		{
			using namespace std;
			ifstream in{ filename, ios::binary };
			auto reader = make_binary_reader(in);
			reader.ignore(4 + 4 + 1 + 8);
			reader.read<wstring>();
			int beatmap_count = reader.read<int32_t>();
			vector<InfoType> info_list{};
#if (defined(DEBUG) || defined(_DEBUG)) && defined(THUOSU_BEATMAP_INFO_LIMIT)
			static_assert(std::is_integral<decltype(THUOSU_BEATMAP_INFO_LIMIT)>::value && THUOSU_BEATMAP_INFO_LIMIT > 0,
				"THUOSU_BEATMAP_INFO_LIMIT must be positive integer");
			for (int i = 0; i < std::min(THUOSU_BEATMAP_INFO_LIMIT, beatmap_count); ++i)
#else
			while (beatmap_count--)
#endif
			{
				info_list.push_back(InfoType{});
				auto & info = info_list.back();

				using namespace accessor;

				get_artist_romanized(reader, info);
				get_artist(reader, info);
				get_title_romanized(reader, info);
				get_title(reader, info);
				get_creator(reader, info);
				get_difficulty(reader, info);
				get_music_file(reader, info);
				get_beatmap_md5(reader, info);
				get_beatmap_file(reader, info);
				get_rank_status(reader, info);
				get_circle_count(reader, info);
				get_slider_count(reader, info);
				get_spinner_count(reader, info);
				get_last_edit_time(reader, info);
				get_ar(reader, info);
				get_cs(reader, info);
				get_hp(reader, info);
				get_od(reader, info);
				get_slider_velocity(reader, info);

				// unknown block
				for (int k = 0; k < 4; ++k)
				{
					int count = reader.read<int32_t>();
					if (count < 0)
						throw range_error("invalid value");
					while (count--)
					{
						if (reader.read<int8_t>() != 0x08)
							throw runtime_error("read osu!.db failed");
						reader.ignore(4);  // ??
						if (reader.read<int8_t>() != 0x0D)
							throw runtime_error("read osu!.db failed");
						reader.ignore(8);  // ??
					}
				}

				get_drain_time(reader, info);
				get_total_time(reader, info);
				get_preview_time(reader, info);

				// time point block
				int time_point_count = reader.read<int32_t>();
				if (time_point_count < 0)
					throw range_error("time point count cannot be negative");
				while (time_point_count--)
					reader.ignore(8 + 8 + 1);  // [beat length | rate] + offset + is red line

				get_beatmap_id(reader, info);
				get_beatmap_set_id(reader, info);
				get_beatmap_thread_id(reader, info);
				reader.ignore(4); // ??
				get_user_offset(reader, info);
				get_stack_leniency(reader, info);
				get_play_mode(reader, info);
				get_source(reader, info);
				get_tags(reader, info);
				get_online_offset(reader, info);
				get_title_with_font(reader, info);
				reader.ignore(1 + 8 + 1);
				get_location(reader, info);
				reader.ignore(8 + 5 + 4 + 1);
			}
			if (assign_romanized)
			{
				using namespace accessor;
				for (auto & info : info_list)
				{
					assign_artist(info);
					assign_title(info);
				}
			}
			return info_list;
		}

		//!\brief convert .Net ticks to time_t (ignore subsecond duration)
		//! only works on platforms on which time_t is corresponding to POSIX time with Unix Epoch
		std::time_t ticks_to_time_t(std::int64_t ticks)
		{
			const std::uint64_t unix_epoch = 0x089f7ff5f7b58000uL;
			return (time_t)((ticks - unix_epoch) / 10000000);
		}
	}
}

#endif // THUOSU_BEATMAP_INFO_HPP