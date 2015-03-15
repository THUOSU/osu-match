extern "C"
{
#define	_CRT_SECURE_NO_WARNINGS
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

#include <locale>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <memory>
#include <utility>
#include <algorithm>

namespace detail
{
	struct av_register_guard
	{
		av_register_guard()
		{
			av_register_all();
		}
	};
	static av_register_guard s_av_register_guard;

	template <typename Func>
	struct scope_exit_guard
	{
		Func func;
		scope_exit_guard(Func&& func) : func{ std::forward<Func>(func) } { }
		~scope_exit_guard() { func(); }
	};
	template <typename Func>
	inline scope_exit_guard<Func> make_scope_exit_guard(Func&& func)
	{
		return scope_exit_guard < Func > { std::forward<Func>(func) };
	}
}
#define SCOPE_EXIT_ARG_NAME_CAT(name, line) name##line
#define SCOPE_EXIT_ARG_NAME(name, line) SCOPE_EXIT_ARG_NAME_CAT(name, line)
#define ON_SCOPE_EXIT(func) auto SCOPE_EXIT_ARG_NAME(_EXIT_GUARD_, __LINE__) = ::detail::make_scope_exit_guard(func)

enum class audio_sample_format
{
	u8,
	s16,
	s32,
	f32,
	f64
};

class audio_channel
{
	std::unique_ptr<uint8_t[]> buf_;
	uint8_t *first_, *last_, *end_;
	audio_sample_format format_;
	friend class audio_data;
public:
	audio_channel()
		: buf_(new uint8_t[4]),
		first_(buf_.get()), last_(buf_.get()), end_(buf_.get() + 4),
		format_(audio_sample_format::s16)
	{ }
	audio_channel(const audio_channel &) = delete;
	audio_channel & operator= (const audio_channel &) = delete;

	audio_channel(audio_channel && other)
		: buf_(std::move(other.buf_)),
		first_(other.first_), last_(other.last_), end_(other.end_)
	{ }

	audio_channel & operator= (audio_channel && other)
	{
		buf_ = std::move(other.buf_);
		first_ = other.first_;
		last_ = other.last_;
		end_ = other.end_;
		return *this;
	}

	void append(const uint8_t* data, std::size_t count)
	{
		auto size = last_ - first_, cap = end_ - first_;
		if (count > end_ - last_)
		{
			auto new_cap = cap;
			while (new_cap < count + size)
				new_cap = (new_cap >= 65536 ? new_cap * 2 : new_cap * 4);
			std::unique_ptr<uint8_t[]> tmp(new uint8_t[new_cap]);
			for (auto ps = first_, pd = tmp.get(); ps < last_; ++ps, ++pd)
				*pd = *ps;
			buf_.swap(tmp);
			first_ = buf_.get();
			last_ = buf_.get() + size;
			end_ = buf_.get() + new_cap;
		}
		for (auto pd = data; pd < data + count; ++last_, ++pd)
			*last_ = *pd;
	}

	std::size_t size() const
	{
		return last_ - first_;
	}

	const uint8_t * data() const
	{
		return buf_.get();
	}

	audio_sample_format format() const
	{
		return format_;
	}
};

class audio_data
{
	std::vector<audio_channel> channels_;
	int sample_rate_;
	audio_sample_format sample_format_;
public:
	audio_data(int sample_rate, audio_sample_format sample_format, int channel_count)
		: sample_rate_(sample_rate), sample_format_(sample_format), channels_(channel_count)
	{
		for (auto & c : channels_)
			c.format_ = sample_format;
	}

	audio_data(const audio_data &) = delete;
	audio_data & operator= (const audio_data &) = delete;

	audio_data(audio_data && other)
		: channels_(std::move(other.channels_)),
		sample_rate_(other.sample_rate_), sample_format_(other.sample_format_)
	{ }

	audio_data & operator= (audio_data && other)
	{
		channels_ = std::move(other.channels_);
		sample_rate_ = other.sample_rate_;
		sample_format_ = other.sample_format_;
		return *this;
	}

	size_t channels() { return channels_.size(); }
	audio_channel & channel(size_t index) { return channels_.at(index); }
	audio_channel & operator[] (size_t index) { return channels_[index]; }
	int sample_rate() const { return sample_rate_; }

};

class libav_error final : public std::runtime_error
{
	int error_code;
public:
	libav_error(const std::string & msg, int error = 0) : runtime_error(msg), error_code(error) { }
	libav_error(const char * msg, int error = 0) : runtime_error(msg), error_code(error) { }
	int get_error_code() const { return error_code; }
};

static audio_data read_audio(const std::string & filename)
{
	int ret;

	AVFormatContext * p_fmt_ctx = nullptr;
	if ((ret = avformat_open_input(&p_fmt_ctx, filename.c_str(), nullptr, nullptr)) < 0)
		throw libav_error("Cannot open input file", ret);
	ON_SCOPE_EXIT([&]{ avformat_close_input(&p_fmt_ctx); });

	// find stream info
	if ((ret = avformat_find_stream_info(p_fmt_ctx, nullptr)) < 0)
		throw libav_error("Cannot find stream information", ret);
	av_dump_format(p_fmt_ctx, 0, filename.c_str(), 0);

	// find audio stream
	AVStream * p_stream = nullptr;
	for (auto i = 0U; i < p_fmt_ctx->nb_streams; ++i)
	{
		auto p = p_fmt_ctx->streams[i];
		if (p->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			p_stream = p;
			break;
		}
	}
	if (!p_stream)
		throw libav_error("Audio stream not found");

	// open decoder
	auto p_codec_ctx = p_stream->codec;
	if ((ret = avcodec_open2(p_codec_ctx, avcodec_find_decoder(p_codec_ctx->codec_id), nullptr)) < 0)
		throw libav_error("Failed to open decoder for stream", ret);
	ON_SCOPE_EXIT([&]{ avcodec_close(p_codec_ctx); });

	audio_sample_format sample_format;
	switch (p_codec_ctx->sample_fmt)
	{
		case AV_SAMPLE_FMT_U8: case AV_SAMPLE_FMT_U8P:
			sample_format = audio_sample_format::u8; break;
		case AV_SAMPLE_FMT_S16: case AV_SAMPLE_FMT_S16P:
			sample_format = audio_sample_format::s16; break;
		case AV_SAMPLE_FMT_S32: case AV_SAMPLE_FMT_S32P:
			sample_format = audio_sample_format::s32; break;
		case AV_SAMPLE_FMT_FLT: case AV_SAMPLE_FMT_FLTP:
			sample_format = audio_sample_format::f32; break;
		case AV_SAMPLE_FMT_DBL: case AV_SAMPLE_FMT_DBLP:
			sample_format = audio_sample_format::f64; break;
		default:
			throw std::invalid_argument("Invalid sample format");
	}

	bool is_planar = (p_codec_ctx->sample_fmt == AV_SAMPLE_FMT_U8P
		|| p_codec_ctx->sample_fmt == AV_SAMPLE_FMT_S16P
		|| p_codec_ctx->sample_fmt == AV_SAMPLE_FMT_S32P
		|| p_codec_ctx->sample_fmt == AV_SAMPLE_FMT_FLTP
		|| p_codec_ctx->sample_fmt == AV_SAMPLE_FMT_DBLP
		);
	audio_data result(p_codec_ctx->sample_rate, sample_format, p_codec_ctx->channels);

	AVPacket packet;
	packet.data = nullptr;
	packet.size = 0;

	int data_size = av_get_bytes_per_sample(p_codec_ctx->sample_fmt);
	while (av_read_frame(p_fmt_ctx, &packet) == 0)
	{
		ON_SCOPE_EXIT([&]{ av_free_packet(&packet); });

		auto p_frame = av_frame_alloc();
		if (!p_frame)
			throw libav_error("Failed to alloc frame");
		ON_SCOPE_EXIT([&]{ av_frame_free(&p_frame); });

		int got_frame;
		if ((ret = avcodec_decode_audio4(p_codec_ctx, p_frame, &got_frame, &packet)) < 0)
			throw libav_error("Failed to decode audio packet");

		if (got_frame)
		{
			if (is_planar)
			{
				for (auto ch = 0; ch < p_frame->channels; ++ch)
					result[ch].append(p_frame->extended_data[ch], data_size * p_frame->nb_samples);
			}
			else
			{
				auto p_data = p_frame->data[0];
				for (auto i = 0; i < p_frame->nb_samples; ++i)
				{
					for (auto ch = 0; ch < p_frame->channels; ++ch)
					{
						result[ch].append(p_data, data_size);
						p_data += data_size;
					}
				}
			}
		}
	}

	return result;
}
