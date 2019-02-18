#include <cstdint>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <math.h>
#include <random>
#include <array>

template<int N>
class Block{
	char data[N];
	int cursor = 0;
public:
	constexpr uint32_t size(){return N;};
	Block &operator<<(const char *);
	template<class T>
	Block &operator<<(T);
	int write(int);
};

unsigned char bitmap_data[1000000];

template<int N, int M>
class Heap{
	std::array<int, N> data;
public:
	Heap(){
		std::random_device rnd;
		for(int &i : data){
			i = rnd() % (M);
		}
	}
	void make(){
		for(int i = 1; i < N; ++i){
			int tmp = data[i];
			for(int j = i; j > 0; j = (j - 1) / 2){
				if(data[j] < data[(j - 1) / 2]){
					data[j] = data[(j - 1) / 2];
					data[(j - 1) / 2] = tmp;
				}else break;
			}
		}
	}
	void copy(unsigned char bitmap_data[]){
		for(int i = 0; i < N; ++i) bitmap_data[(data[i] * 499 / (M - 1)) * 188 + (i * 1499 / (N - 1)) / 8] |= 1 << (7 - (i * 1499 / (N - 1)) % 8);
	}
};


int main(void){
	uint16_t width = 1500, height = 500;
	Block<14> bitmapfileheader;
	Block<12> bitmapcoreheader;
	Block<6> color_palette;
	int fd = creat("out.bmp", S_IWUSR | S_IRUSR);
	bitmapfileheader << "BM";
	bitmapfileheader << bitmapfileheader.size() + bitmapcoreheader.size() + color_palette.size() + width * height;
	bitmapfileheader << uint16_t(0) << uint16_t(0);
	bitmapfileheader << bitmapfileheader.size() + bitmapcoreheader.size() + color_palette.size();
	bitmapcoreheader << bitmapcoreheader.size() << width << height << uint16_t(1) << uint16_t(1);
	color_palette << uint8_t(0) << uint8_t(0) << uint8_t(0);
	color_palette << uint8_t(255) << uint8_t(255) << uint8_t(255);

	Heap<1000000, 100000> heap;
	heap.make();
	heap.copy(bitmap_data);

	bitmapfileheader.write(fd);
	bitmapcoreheader.write(fd);
	color_palette.write(fd);
	write(fd, bitmap_data, width * height);
}

template<int N>
Block<N> &Block<N>::operator<<(const char *a){
	int i;
	for(i = 0; a[i]; ++i) data[cursor + i] = a[i];
	cursor += i;
	return *this;
}

template<int N>
template<class T>
Block<N> &Block<N>::operator<<(T a){
	memcpy(data + cursor, &a, sizeof a);
	cursor += sizeof a;
	return *this;
}

template<int N>
int Block<N>::write(int fd){
	::write(fd, data, N);
	return N;
}

