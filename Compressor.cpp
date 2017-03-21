#include "provided.h"
#include "HashTable.h"
#include <string>
#include <vector>
using namespace std;

void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	unsigned int capacity = s.length() / 2 + 512;
	if (capacity > 16384) capacity = 16384;
	HashTable<string, unsigned short> ht(capacity * 2, capacity);
	for (unsigned short j = 0; j != 256; j++)
		ht.set(string(1, static_cast<char>(j)), j, true);
	unsigned short nextFreeID = 256;
	string runSoFar;
	vector<unsigned short> result;
	for (auto c : s)
	{
		string expandedRun = runSoFar + c;
		unsigned short erVal;
		if (ht.get(expandedRun, erVal))
		{
			runSoFar = expandedRun;
			continue;
		}
		unsigned short x;
		ht.get(runSoFar, x);
		result.push_back(x);
		ht.touch(runSoFar);
		runSoFar = "";
		unsigned short cv;
		ht.get(string(1, c), cv);
		result.push_back(cv);
		if (!ht.isFull())
		{
			ht.set(expandedRun, nextFreeID);
			nextFreeID++;
		}
		else
		{
			string discardedKey;
			unsigned short discardedVal;
			ht.discard(discardedKey, discardedVal);
			ht.set(expandedRun, discardedVal);
		}
	}
	if (!runSoFar.empty())
	{
		unsigned short x;
		ht.get(runSoFar, x);
		result.push_back(x);
	}
	result.push_back(capacity);
	numbers = result;
}

bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	if (numbers.empty()) return false;
	unsigned int capacity = numbers[numbers.size() - 1];
	HashTable<unsigned short, string> ht(capacity * 2, capacity);
	for (size_t j = 0; j != 256; j++)
		ht.set(j, string(1, static_cast<char>(j)), true);
	unsigned short nextFreeID = 256;
	string runSoFar;
	string output;
	for (unsigned int k = 0; k != numbers.size() - 1; k++)
	{
		unsigned short us = numbers[k];
		if (us <= 255)
		{
			string append;
			ht.get(us, append);
			output += append;
			if (runSoFar.empty())
			{
				runSoFar = append;
				continue;
			}
			string expandedRun = runSoFar + append;
			if (!ht.isFull())
			{
				ht.set(nextFreeID, expandedRun);
				nextFreeID++;
			}
			else
			{
				unsigned short discardedKey;
				string discardedString;
				ht.discard(discardedKey, discardedString);
				ht.set(discardedKey, expandedRun);
			}
			runSoFar = "";
			continue;
		}
		else
		{
			string s;
			if (!ht.get(us, s))
				return false;
			ht.touch(us);
			output += s;
			runSoFar = s;
		}
	}
	s = output;
	return true;
}

unsigned int computeHash(string s)
{
	// FNV Algorithm
	unsigned int h = 2166136261U;
	for (auto c : s)
	{
		h += c;
		h *= 16777619;
	}
	return h;
}

unsigned int computeHash(unsigned short i)
{
	return i;
}