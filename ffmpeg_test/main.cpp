#include "audio_io.hpp"

#include <fstream>
#include <vector>

int main(int argc, char **argv)
{
	using namespace std;

	auto data = read_audio("test.mp3");
	//auto data = input_file(R"(test.mp3)");
	//ofstream filel(R"(testl.pcm)", ios::binary);
	//filel.write(reinterpret_cast<const char*>(data.channel(0).data()), data.channel(0).size());
	//ofstream filer(R"(testr.pcm)", ios::binary);
	//filer.write(reinterpret_cast<const char*>(data.channel(1).data()), data.channel(0).size());
	int sizes[] = { 1, 2, 4, 4, 8 };
	ofstream file(R"(test.pcm)", ios::binary);
	const char* p1 = reinterpret_cast<const char*>(data.channel(0).data());
	const char* p2 = reinterpret_cast<const char*>(data.channel(1).data());
	vector<char> out;
	out.reserve(data.channel(0).size() * 2);
	auto ins = back_inserter(out);
	auto data_size = sizes[static_cast<int>(data.channel(0).format())];
	for (auto i = 0u; i < data.channel(0).size() / data_size; ++i)
	{
		//file.write(p1, 2);
		//file.write(p2, 2);
		//p1 += 2;
		//p2 += 2;
		for (int j = 0; j < data_size; ++j)
			*ins++ = *p1++;
		for (int j = 0; j < data_size; ++j)
			*ins++ = *p2++;
	}
	file.write(out.data(), out.size());
	system("pause");
	return 0;
}