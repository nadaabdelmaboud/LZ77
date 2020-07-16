
#define _CRT_SECURE_NO_DEPRECATE

#include<iostream>
#include<vector>
#include<string>
#include <climits>
#include<fstream>
#include<set>
#include <bitset>
#include <sstream>
#include <cmath>
#include<unordered_map>
#include <iterator>
#include <sys/stat.h>
using namespace std;
string text="" ;
string encodedBinary = "";
FILE *MyReadFile; 
int largestLength = 0;
int largestOffset = 0;
void readFiles() {
	text = "";
	MyReadFile = fopen("input.txt", "r");
	while (!feof(MyReadFile)) {
		char c = fgetc(MyReadFile);
		text += c;

	}
}

int windowSize ;
int searchBufferSize ;
int currentSearchBufferSize = 0;

int lookBufferSize;
int startIndex = 0;

int searchPointer = -1;
int lookPointer = 0;
struct triplet {
	int offset;
	int length;
	char symbol;
};
vector<triplet>decoded;
// <offset,length,first unmatched symbol>
vector<triplet>codes;
void encode() {
	
	while (lookPointer< text.length()) {
		lookPointer = startIndex + currentSearchBufferSize;
		windowSize = (text.length() - startIndex >= windowSize) ? windowSize : text.length() - startIndex;
		lookBufferSize = windowSize - currentSearchBufferSize;
			int length = 0;
			int offset = -1;
			int maxOffset = offset;
			int minOffset = 0;
			int maxLength = 0;
			int tmplookPointer = lookPointer;
			int temp2= lookPointer;
			searchPointer = (searchPointer != -1) ? startIndex : -1;
			while (searchPointer < currentSearchBufferSize+ startIndex) {
				if (searchPointer == -1) {
					codes.push_back({ 0,0,text[lookPointer] });
					currentSearchBufferSize++;
					tmplookPointer++;
					searchPointer = startIndex;
					break;
				}
				length = 0;
				offset = -1;
				tmplookPointer = lookPointer;
				bool in = 0;
				while (tmplookPointer < (startIndex+windowSize) && searchPointer < startIndex+currentSearchBufferSize && text[tmplookPointer] == text[searchPointer]) {
					if (length == 0)offset = (searchPointer);
					length++;
					searchPointer++;
					tmplookPointer++;
					in = 1;
				}

				if (length >= maxLength) {
					if (maxLength == length) {
						if(maxOffset<offset)
							minOffset = (currentSearchBufferSize+ startIndex) - offset-1;

					}
					else {
						maxOffset = offset;
						minOffset= (currentSearchBufferSize + startIndex) - offset - 1;
					}
					maxLength = length;
					temp2 = lookPointer;
					temp2 += (maxLength);
					if (maxLength > largestLength) { largestLength = maxLength; }
					if (minOffset > largestOffset) { largestOffset = minOffset; }

				}
				if(!in) searchPointer++;
				
			}
			if (searchPointer!=0&&maxLength==0 && currentSearchBufferSize<searchBufferSize&&tmplookPointer<text.length()) {
				char symbol = (temp2 >= text.length()) ?  '$'  : text[temp2];
				codes.push_back({ 0,0,symbol });
				currentSearchBufferSize++;
			}
			else if (searchPointer != 0 && maxLength != 0 && currentSearchBufferSize < searchBufferSize&& tmplookPointer < text.length()) {
				char symbol = (temp2 >= text.length()) ?  '$'  : text[temp2];
				codes.push_back({ minOffset,maxLength,symbol });
				currentSearchBufferSize += (maxLength + 1);

			}
			else if (searchPointer != 0 && maxLength == 0) {
				char symbol = (temp2 >= text.length()) ?  '$'  : text[temp2];
				codes.push_back({ 0,0,symbol });
				startIndex = startIndex + maxLength + 1;
			}

			else if (searchPointer != 0 && maxLength != 0 ) {
				char symbol = (temp2 >= text.length()) ?  '$'  : text[temp2];
				codes.push_back({ minOffset,maxLength,symbol });
				startIndex += (maxLength + 1);
			}
		}




	
	}
string decoedtext = "";
void decode() {
	for (int i = 0;i < decoded.size()-1;i++) {
		string str="";
		if (decoded[i].length == 0) {
			str += decoded[i].symbol;
		}
		else {
			int size = decoedtext.length()- decoded[i].offset-1;
			str += decoedtext.substr(size, decoded[i].length) + decoded[i].symbol;
		}
		
		decoedtext += str;

	}
	FILE *t;
	char c;
	t = fopen("decoded.txt", "w");
	for (int i = 0;i < decoedtext.size()-1 ;i++)
	{
		char c = decoedtext[i];
		fputc(c, t);
	}

	fclose(t);
}
unsigned noOfBits(unsigned int n)
{
	if (n == 0) { return 1; }
	unsigned int out = (int)log2(n) + 1;
	return out;
}


char convertStringChar(string x) {
	string reversed;
	for (int i = x.length()-1;i >=0;i--) {
		reversed.push_back(x[i]);
	}
	std::istringstream str(reversed);
	std::bitset<8> bits;
	char c1;
	while (str >> bits)
		c1= char(bits.to_ulong());
	return c1;
}
string convertInteger(int n, int s) {
	string out1;
	for (int i = 0;i < s;i++) {
		bool is_set = n & (1 << i);
		(is_set == 1) ? out1 += '1' : out1 += '0';
	}
	string out;
	for (int i = out1.length() - 1;i >= 0;i--) {
		out.push_back(out1[i]);
	}
	return out;
}
int convertString(string s) {
	int num = 0;
	int count = 0;
	for (int i = s.length()-1;i >= 0;i--)
	{
		bool bit = (s[i] == '1') ? 1 : 0;
		num |= (bit << count);
		count++;
	}
	return num;
}
int main() {

	cout << "enter window size "<< endl;
	cin>>windowSize ;
	cout << "enter search-buffer size"<< endl;
	cin>>searchBufferSize;
	readFiles();
	cout << "reading input "<< endl;
	cout << "text length  " <<text.length()<< endl;
	cout << "start encode" << endl;
	encode();

	ofstream outfile("encode.bin", ios::out | ios::binary);
	unsigned int intSize = noOfBits(largestLength);
	string p = ""; 
	for (int i=0;i<codes.size();i++)
	{
		p = "";
		for (int j = 0;j < 3;j++) {
			if (j == 0) { 
				intSize = noOfBits(largestOffset);
				p += convertInteger(codes[i].offset, intSize);
			
			}
			else if(j==1) {
				intSize = noOfBits(largestLength);
				p += convertInteger(codes[i].length, intSize);
			}
			else 
			{

				char c= (char)codes[i].symbol;
				for (int i = 0; i < 8; ++i) 
				{
					bool is_set = c & (1 << i);
					(is_set == 1) ? p += '1' : p += '0';

				}
				
			}
		}
		encodedBinary += p;
	}
	int numzeros = 0;
	if (encodedBinary.size() % 8 != 0) {
		int count = 8 - encodedBinary.size() % 8;
		while (count--) {
			numzeros++;
			encodedBinary += '0';
		}

	}
	for (long long i = 0; i < long(encodedBinary.size()); i += 8)
	{
		int b = 0;
		for (long long j = 0;j < 8;j++) {
			int bit = (encodedBinary[i + j] == '1') ? 1 : 0;
			b |= (bit << j);

		}
		char c = (char)(b & 255);
		outfile.write(&c, 1);
	}
	outfile.close();

	cout << "start decode" << endl;
	string encstring = "";
	FILE* EncFile = fopen("encode.bin", "rb");
	while (!feof(EncFile)) {
		char c = fgetc(EncFile);

		for (int i = 0; i < 8; ++i) {
			bool is_set = c & (1 << i);
			(is_set == 1) ? encstring += '1' : encstring += '0';

		}

	}

	encstring = encstring.substr(0, encstring.length() - 8);
	int cx = 0;
	triplet tripl;
	int countWhile = 0;
	while (countWhile < encstring.length()) {
		string num = "";
		if (cx == 1) {
			intSize = noOfBits(largestLength);
		}
		else if (cx == 0) {
			intSize = noOfBits(largestOffset);
		}
		else if (cx == 2) {
			intSize = 8;
		}
		for (int j = 0;j < intSize;j++) {
			if(countWhile + j< encstring.length())
			num += encstring[countWhile + j];
		}
		int code = convertString(num);
		if (cx == 0) {
			tripl.offset = code;
			cx++;
		}
		else if (cx == 1) {
			tripl.length = code;
			cx++;
			
		}
		else if (cx == 2) {
			char symb = convertStringChar(num);
			tripl.symbol = symb;
			decoded.push_back(tripl);
			cx = 0;
		}

		countWhile += intSize;
	}


	decode();

	system("PAUSE");
}

