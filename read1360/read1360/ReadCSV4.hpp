
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
	string dataFile;    //�����ļ���

	//��ά������Ϣ
	int x;
	int y;
	int z;

	int lineCount = 0;
public:
	ReadCSV(string path, int x1, int y1, int z1) : dataFile(path), x(x1), y(y1), z(z1) {}
	ReadCSV() :ReadCSV(string(), 0, 0, 0) {}

	/*�������ݳ�Ա*/
	void SetFilePath(string path) { dataFile = path; }        //�����ļ���
	void SetX(int x1) { x = x1; }
	void SetY(int y1) { y = y1; }
	void SetZ(int z1) { z = z1; }

	/*��ӡ����ʱ��*/
	static time_t printTime(const char* str) {
		time_t now = time(nullptr);
		char* curr_time = ctime(&now);
		std::cout << str << curr_time << "---" << now << endl << endl;
		return now;
	}

	/* ��ȡ������
	   ������������������
	*/
	int work(vector<vector<vector<int>>>& content)
	{
		if (x && y && z)      //��ʼ����������
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
		ifs.open(dataFile.data());        //���ļ�
		if (ifs.is_open()) {
			cout << dataFile << " opening successful." << endl;
		}
		else {
			cout << dataFile << " opening failed" << endl;
			return NULL;
		}

		int len = 1024 * 1024 * 4;    //�ڴ���С
		char* mem = new char[len];    //�����ڴ�
		if (mem == NULL) {
			printf("no mem!\n");
			return NULL;
		}

		time_t start_time = printTime("������ʼ: ");

		streamsize totalCount = 0;      //�����ܵĽ�������
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

			streamsize parse_count = 0;     //���浥�ν�������
			bool full = false;
			parse(mem, content, parse_count, full);
			totalCount += parse_count;

			if (full)break;
		}
		if (mem) delete[] mem;     //�ͷ��ڴ�

		time_t end_time = printTime("������:  ");
		cout << "һ������ " << lineCount << " ��" << totalCount << " ������  ��ʱ��" << (end_time - start_time) << "s" << endl;
	}


private:

	streamsize Read(istream& stream, char* buffer, streamsize count)
	{
		//rdbuf() ����ʵ��һ��������ָ�����������һ�������������
		//sgetn() ���سɹ���ȡ���ַ���
		streamsize reads = stream.rdbuf()->sgetn(buffer, count);

		//rdstate() ����״̬��
		stream.rdstate();

		//peek() �书���Ǵ��������ж�ȡһ���ַ� �����ַ���δ����������ɾ��������ֵ�����ж��Ƿ����ĩβ
		stream.peek();
		return reads;
	}

	/*  ��ȡ��
		�������ڴ�飬���С���ļ�������
	*/
	int readBlocks(char* mem, streamsize size, ifstream& in)
	{
		char buff[4096]{ 0 };
		char* p = mem;
		streamsize readSize = 0;        //�Ѷ�ȡ�ֽ���
		streamsize target_size = size < (streamsize)sizeof(buff) ? size : (streamsize)sizeof(buff);     //���ȡĿ���С����������ȡ4096�ֽ�

		while (true) {
			//streamsize ret = 0;
			//if (in.read(p, target_size)) ret = in.gcount();      //�����ж�ȡtarget_size��ret���ʵ�ʶ�ȡ���ֽ���
			streamsize ret = Read(in, p, target_size);
			if (ret <= 0)
			{
				streamsize a = sizeof(buff);
				streamsize target_size1 = size < (streamsize)sizeof(buff) ? size : (streamsize)sizeof(buff);
				p = nullptr; break;     //��ȡ���󣬷���
			}

			readSize += ret;                 //�Ѿ��������ֽ��� + ret
			if (readSize >= size) break;     //��������

			if ((size - readSize) < target_size) target_size = size - readSize;     //�����Ҫ��ȡ���ֽ��� < target_size
			p += ret;            //ָ��ƫ��ret���ֽڼ�����һ�α���
		}

		char* p_end = mem + readSize;
		if (*p_end == '\0') return NULL;      //���һ���ֽ����ַ���������ֱ�ӷ���

		while (*p_end != '\n') p_end--;
		p_end++;
		*p_end = '\0';     //�����һ�� '\n'֮����'\0'

		streamsize pos = mem + readSize - p_end;    //��������ƫ�Ƶ�λ��
		in.seekg(-pos, ios::cur);       //�ļ�����λ������ƫ�ƺ�λ��

		return NULL;
	}

	/*  ������
		�ڴ�飬������������ȡ�ѽ�����������ȡ�����Ƿ�����
	*/
	int parse(char*& mem, vector<vector<vector<int>>>& content, streamsize& parseCount, bool& full)
	{
		char* p = mem;
		int num = 0;      //�ѽ���������
		static int x2 = 0, y2 = 0, z2 = 0;
		while (*p)
		{
			num++;
			int data = atoi((char*)p);
			content[x2][y2][z2] = data;

			if (++z2 >= z)      //z��ȫ������
			{
				z2 = 0;
				if (++y2 >= y)      //y��ȫ������
				{
					y2 = 0;
					if (++x2 >= x)   //x��ȫ������
					{
						cout << "vector full!" << endl;
						full = true;
						parseCount = num;
						return NULL;
					}
				}
			}

			while (*p != ',' && *p != '\n') p++;   //������ǰ����

			if (*p == '\n') lineCount++;
			p++;    //����',' or '\n'
		}
		parseCount = num;
		return NULL;
	}
};

#endif