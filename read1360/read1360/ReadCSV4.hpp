
#ifndef _READCSV_H_
#define _READCSV_H_

#define _CRT_SECURE_NO_WARNINGS

#include<string>
#include<fstream>
#include<iostream>
#include<vector>
#include<sstream>
#include<ctime>

using namespace std;

class ReadCSV {
private:
	string dataFile;    //保存文件名

	//三维数组信息
	int x;
	int y;
	int z;

	int lineCount = 0;
public:
	ReadCSV(string path, int x1, int y1, int z1) : dataFile(path), x(x1), y(y1), z(z1) {}
	ReadCSV() :ReadCSV(string(), 0, 0, 0) {}

	/*设置数据成员*/
	void SetFilePath(string path) { dataFile = path; }        //设置文件名
	void SetX(int x1) { x = x1; }
	void SetY(int y1) { y = y1; }
	void SetZ(int z1) { z = z1; }

	/*打印调试时间*/
	static time_t printTime(const char* str) {
		time_t now = time(nullptr);
		char* curr_time = ctime(&now);
		std::cout << str << curr_time << "---" << now << endl << endl;
		return now;
	}

	/* 读取并解析
	   参数：保存数据容器
	*/
	int work(vector<vector<vector<int>>>& content)
	{
		if (x && y && z)      //初始化数组容器
		{
			content.resize(x);
			for (int i = 0; i < x; i++)
			{
				content[i].resize(y);
				for (int j = 0; j < y; j++)
				{
					content[i][j].resize(z);
				}
			}
		}

		ifstream ifs;
		ifs.open(dataFile.data());        //打开文件
		if (ifs.is_open()) {
			cout << dataFile << " opening successful." << endl;
		}
		else {
			cout << dataFile << " opening failed" << endl;
			return NULL;
		}

		int len = 1024 * 1024 * 4;    //内存块大小
		char* mem = new char[len];    //申请内存
		if (mem == NULL) {
			printf("no mem!\n");
			return NULL;
		}

		time_t start_time = printTime("解析开始: ");

		streamsize totalCount = 0;      //保存总的解析个数
		streamsize num = 128 * 1024 * 1359;
		while (1) {
			if (ifs.eof()) {
				cout << "had read finished\n";
				break;
			}

			/*if (totalCount == num)
			{
				int a = 1;
			}*/

			readBlocks(mem, len, ifs);       //4kb

			streamsize parse_count = 0;     //保存单次解析个数
			bool full = false;
			parse(mem, content, parse_count, full);
			totalCount += parse_count;

			if (full)break;
		}
		if (mem) delete[] mem;     //释放内存

		time_t end_time = printTime("解析完:  ");
		cout << "一共解析 " << lineCount << " 行" << totalCount << " 个数据  耗时：" << (end_time - start_time) << "s" << endl;
	}


private:

	streamsize Read(istream& stream, char* buffer, streamsize count)
	{
		//rdbuf() 可以实现一个流对象指向的内容用另一个流对象来输出
		//sgetn() 返回成功读取的字符数
		streamsize reads = stream.rdbuf()->sgetn(buffer, count);

		//rdstate() 返回状态字
		stream.rdstate();

		//peek() 其功能是从输入流中读取一个字符 但该字符并未从输入流中删除，返回值可以判断是否读到末尾
		stream.peek();
		return reads;
	}

	/*  读取块
		参数：内存块，块大小，文件输入流
	*/
	int readBlocks(char* mem, streamsize size, ifstream& in)
	{
		char buff[4096]{ 0 };
		char* p = mem;
		streamsize readSize = 0;        //已读取字节数
		streamsize target_size = size < (streamsize)sizeof(buff) ? size : (streamsize)sizeof(buff);     //块读取目标大小，单次最大读取4096字节

		while (true) {
			//streamsize ret = 0;
			//if (in.read(p, target_size)) ret = in.gcount();      //从流中读取target_size，ret获得实际读取的字节数
			streamsize ret = Read(in, p, target_size);
			if (ret <= 0)
			{
				streamsize a = sizeof(buff);
				streamsize target_size1 = size < (streamsize)sizeof(buff) ? size : (streamsize)sizeof(buff);
				p = nullptr; break;     //读取错误，返回
			}

			readSize += ret;                 //已经读到的字节数 + ret
			if (readSize >= size) break;     //大于跳出

			if ((size - readSize) < target_size) target_size = size - readSize;     //最后需要读取的字节数 < target_size
			p += ret;            //指针偏移ret个字节继续下一次保存
		}

		char* p_end = mem + readSize;
		if (*p_end == '\0') return NULL;      //最后一个字节是字符串结束符直接返回

		while (*p_end != '\n') p_end--;
		p_end++;
		*p_end = '\0';     //在最后一个 '\n'之后置'\0'

		streamsize pos = mem + readSize - p_end;    //计算往后偏移的位数
		in.seekg(-pos, ios::cur);       //文件流定位到最终偏移后位置

		return NULL;
	}

	/*  解析块
		内存块，数据容器，获取已解析个数，获取容器是否已满
	*/
	int parse(char*& mem, vector<vector<vector<int>>>& content, streamsize& parseCount, bool& full)
	{
		char* p = mem;
		int num = 0;      //已解析数个数
		static int x2 = 0, y2 = 0, z2 = 0;
		while (*p)
		{
			num++;
			int data = atoi((char*)p);
			content[x2][y2][z2] = data;

			if (++z2 >= z)      //z轴全部置数
			{
				z2 = 0;
				if (++y2 >= y)      //y轴全部置数
				{
					y2 = 0;
					if (++x2 >= x)   //x轴全部置数
					{
						cout << "vector full!" << endl;
						full = true;
						parseCount = num;
						return NULL;
					}
				}
			}

			while (*p != ',' && *p != '\n') p++;   //跳过当前数字

			if (*p == '\n') lineCount++;
			p++;    //跳过',' or '\n'
		}
		parseCount = num;
		return NULL;
	}
};

#endif