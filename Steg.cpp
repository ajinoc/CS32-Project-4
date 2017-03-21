#include "provided.h"
#include <string>
#include <vector>
using namespace std;

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut) 
{
	if (hostIn.empty()) return false;

	vector<string> lines;
	for (size_t k = 0; k < hostIn.length(); k++)
	{
		string curLine;
		while (k != hostIn.length() && hostIn[k] != '\n')
		{
			if (hostIn[k] == '\r' && k + 1 != hostIn.length() && hostIn[k + 1] == '\n')
			{
				k++;
				break;
			}
			curLine += hostIn[k];
			k++;
		}
		lines.push_back(curLine);
	}
	
	for (size_t k = 0; k != lines.size(); k++)
	{
		int i = lines[k].length() - 1;
		while (i >= 0 && (lines[k][i] == '\t' || lines[k][i] == ' '))
			i--;
		lines[k] = lines[k].substr(0, i + 1);
	}

	vector<unsigned short> numbers;
	Compressor::compress(msg, numbers);
	string compMsg = BinaryConverter::encode(numbers);
	vector<string> compMsgSS;

	for (size_t i = 0; i < compMsg.length();)
	{
		string tempSS;
		if (compMsgSS.size() < compMsg.length() % lines.size())
		{
			size_t k = 0;
			for (; k < (compMsg.length() / lines.size()) + 1; k++)
			{
				tempSS += compMsg[i + k];
			}
			compMsgSS.push_back(tempSS);
			i += k;
		}
		else
		{
			size_t k = 0;
			for (; k < (compMsg.length() / lines.size()); k++)
			{
				tempSS += compMsg[i + k];
			}
			compMsgSS.push_back(tempSS);
			i += k;
		}
	}

	for (size_t i = 0; i != compMsgSS.size(); i++)
		lines[i] += compMsgSS[i];

	hostOut = "";
	for (size_t k = 0; k != lines.size(); k++)
		hostOut += lines[k] + '\n';
	return true;
}

bool Steg::reveal(const string& hostIn, string& msg) 
{
	vector<string> lines;
	for (size_t k = 0; k < hostIn.length(); k++)
	{
		string curLine;
		while (k != hostIn.length() && hostIn[k] != '\n')
		{
			if (hostIn[k] == '\r' && k + 1 != hostIn.length() && hostIn[k + 1] == '\n')
			{
				k++;
				break;
			}
			curLine += hostIn[k];
			k++;
		}
		lines.push_back(curLine);
	}

	for (size_t k = 0; k != lines.size(); k++)
	{
		int i = lines[k].length() - 1;
		while (i >= 0 && (lines[k][i] == '\t' || lines[k][i] == ' '))
			i--;
		lines[k] = lines[k].substr(i + 1, lines[k].length());
	}

	string bitString;
	for (size_t k = 0; k != lines.size(); k++)
		bitString += lines[k];

	vector<unsigned short> numbers;
	if (!BinaryConverter::decode(bitString, numbers))
		return false;

	string result;
	if (!Compressor::decompress(numbers, result))
		return false;
	
	msg = result;
	return true;
}
