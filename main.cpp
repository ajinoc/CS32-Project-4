#include "provided.h"
#include "HashTable.h"
#include "http.h"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
using namespace std;

void HashTableTest()
{
	HashTable<string, int> nameToAge(1, 3);
	assert(nameToAge.set("Adam", 19));
	assert(nameToAge.set("Zack", 21));
	assert(nameToAge.set("Dann", 50, true));
	assert(!nameToAge.set("Cheri", 50));
	assert(nameToAge.set("Dann", 45, false));
	assert(nameToAge.isFull());
	string key; int val;
	assert(nameToAge.get("Adam", val) && val == 19);
	assert(nameToAge.discard(key, val) && key == "Adam" && val == 19);
	assert(!nameToAge.isFull());
	assert(!nameToAge.get("Adam", val));
	assert(nameToAge.get("Dann", val) && val == 45);
	assert(nameToAge.discard(key, val) && key == "Zack" && val == 21);
	assert(!nameToAge.get("Zack", val));
	assert(!nameToAge.discard(key, val) && key == "Zack" && val == 21);
	assert(nameToAge.get("Dann", val) && val == 45);
	assert(nameToAge.set("Adam", 19));
	assert(nameToAge.set("Zack", 21));
	assert(nameToAge.touch("Adam"));
	assert(nameToAge.touch("Adam"));
	assert(nameToAge.touch("Zack"));
	assert(nameToAge.touch("Adam"));
	assert(nameToAge.discard(key, val) && key == "Zack" && val == 21);
	assert(nameToAge.discard(key, val) && key == "Adam" && val == 19);
	assert(!nameToAge.discard(key, val) && key == "Adam" && val == 19);
	assert(nameToAge.set("Cheri", 50, true));
	assert(!nameToAge.discard(key, val) && key == "Adam" && val == 19);
	assert(!nameToAge.touch("Cheri") && !nameToAge.touch("Dann"));
	assert(!nameToAge.isFull());
	assert(nameToAge.set("Bobby", 999, true));
	assert(nameToAge.isFull());
	assert(!nameToAge.discard(key, val) && key == "Adam" && val == 19);
	assert(!nameToAge.set("Adam", 19));
	assert(nameToAge.set("Dann", 0));
	assert(nameToAge.set("Dann", 10, false));
	assert(!nameToAge.discard(key, val) && key == "Adam" && val == 19);
	assert(nameToAge.get("Dann", val) && val == 10);
}

void BinaryConverterTest()
{
	vector<unsigned short> numbers;
	numbers.push_back(30);
	numbers.push_back(127);
	numbers.push_back(34);
	numbers.push_back(234);
	numbers.push_back(100);

	string encodedNumbers = BinaryConverter::encode(numbers);

	vector<unsigned short> decodedNumbers;

	if (BinaryConverter::decode(encodedNumbers, decodedNumbers))
		for (size_t k = 0; k != decodedNumbers.size(); k++)
			assert(decodedNumbers[k] == numbers[k]);
}

void CompressorTest()
{
	vector<unsigned short> numbers;
	Compressor::compress("AAAAAAAAAB", numbers);
	vector<unsigned short> result;
	result.push_back(65);
	result.push_back(65);
	result.push_back(256);
	result.push_back(65);
	result.push_back(257);
	result.push_back(65);
	result.push_back(66);
	result.push_back(517);
	assert(numbers.size() == result.size());
	for (size_t k = 0; k != numbers.size(); k++)
		assert(numbers[k] == result[k]);

	string message;
	vector<unsigned short> compressedMessage;
	compressedMessage.push_back(65);
	compressedMessage.push_back(65);
	compressedMessage.push_back(256);
	compressedMessage.push_back(65);
	compressedMessage.push_back(65);
	compressedMessage.push_back(65);
	compressedMessage.push_back(256);
	compressedMessage.push_back(66);
	compressedMessage.push_back(257);
	assert(Compressor::decompress(compressedMessage, message));
	assert(message == "AAAAAAAAAB");

	assert(Compressor::decompress(numbers, message));
	assert(message == "AAAAAAAAAB");
}

void StegTest()
{
	vector<string> lines;
	string testString = "<html>   \r\nQ \tQQ \t \nBBB\t\t\t   \r\n\nGG \nBBB \r\n\t\nDDD\nEEE </html>   ";
	for (size_t k = 0; k < testString.length(); k++)
	{
		string curLine;
		while (k != testString.length() && testString[k] != '\n')
		{
			if (testString[k] == '\r' && k + 1 != testString.length() && testString[k + 1] == '\n')
			{
				k++;
				break;
			}
			curLine += testString[k];
			k++;
		}
		lines.push_back(curLine);
	}
	assert(lines[0] == "<html>   ");
	assert(lines[1] == "Q \tQQ \t ");
	assert(lines[2] == "BBB\t\t\t   ");
	assert(lines[3] == "");
	assert(lines[4] == "GG ");
	assert(lines[5] == "BBB ");
	assert(lines[6] == "\t");
	assert(lines[7] == "DDD");
	assert(lines[8] == "EEE </html>   ");

	for (size_t k = 0; k != lines.size(); k++)
	{
		int i = lines[k].length() - 1;
		while (i >= 0 && (lines[k][i] == '\t' || lines[k][i] == ' '))
			i--;
		lines[k] = lines[k].substr(0, i + 1);
	}
	assert(lines[0] == "<html>");
	assert(lines[1] == "Q \tQQ");
	assert(lines[2] == "BBB");
	assert(lines[3] == "");
	assert(lines[4] == "GG");
	assert(lines[5] == "BBB");
	assert(lines[6] == "");
	assert(lines[7] == "DDD");
	assert(lines[8] == "EEE </html>");

	string hostOut;
	assert(Steg::hide(testString, "Meet me at Ackerman at noon. I am in grave danger, and need your help!", hostOut));
	string result;
	assert(Steg::reveal(hostOut, result));
	assert(result == "Meet me at Ackerman at noon. I am in grave danger, and need your help!");
}

void WebStegTest()
{
	string url = "http://cs.ucla.edu/classes/winter15/cs32/l4/index.html";
	string result;
	
	assert(WebSteg::revealMessageInPage(url, result));
	assert(result == "No question on the CS 32 final exam has 42 as its answer.\n");

	url = "http://www.google.com";
	assert(WebSteg::hideMessageInPage(url, "No question on the CS 32 final exam has 42 as its answer.\n", result));
	HTTP().set(url, result);
	assert(WebSteg::revealMessageInPage(url, result));
	assert(result == "No question on the CS 32 final exam has 42 as its answer.\n");
}

int ClassTestDriver()
{
	string pageText =
		"<html>\n"
		"<head>\n"
		" <title>My Pretend Web Page</title>\n"
		"</head>\n"
		"<body>\n"
		"<h2>My Pretend Web Page<h1>\n"
		"<p>\n"
		"I wish I were creative enough to have something interesting \n"
		"to say here.  I'm not, though, so this is a boring page.\n"
		"</p>\n"
		"<p>\n"
		"Oh, well.\n"
		"</p>\n"
		"</body>\n"
		"</html>\n"
		;
	HTTP().set("http://boring.com", pageText);
	string plan =
		"Lefty and Mugsy enter the bank by the back door.\n"
		"Shorty waits out front in the getaway car.\n"
		"Don't let the teller trip the silent alarm.\n"
		;
	if (!WebSteg::hideMessageInPage("http://boring.com", plan, pageText))
	{
		cout << "Error hiding!" << endl;
		return 1;
	}
	HTTP().set("http://boring.com", pageText);
	string msg;
	if (!WebSteg::revealMessageInPage("http://boring.com", msg))
	{
		cout << "Error revealing!" << endl;
		return 1;
	}
	if (msg != plan)
	{
		cout << "Message not recovered properly:\n" << msg;
		return 1;
	}
	cout << "Recovered text is what was hidden:\n" << msg;
	return 0;
}

int main()
{
	HashTableTest();
	BinaryConverterTest();
	CompressorTest();
	StegTest();
	assert(_CrtDumpMemoryLeaks() == 0); // WebStegTest() causes memory leaks from HTTP
	WebStegTest();
	assert(ClassTestDriver() == 0);

	string url = "http://cs.ucla.edu/classes/winter15/cs32/l4/index.html";
	string result;

	WebSteg::revealMessageInPage(url, result);

	cout << result << endl;

	return 0;
}
