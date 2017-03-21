#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	string result;
	for (size_t k = 0; k != numbers.size(); k++)
		result += convertNumberToBitString(numbers[k]);

	for (size_t k = 0; k != result.size(); k++)
	{
		if (result[k] == '0') result[k] = ' ';
		else if (result[k] == '1') result[k] = '\t';
	}
	return result;
}

bool BinaryConverter::decode(const string& bitString, vector<unsigned short>& numbers)
{
	if (bitString.length() % 16 != 0) return false;
	for (size_t k = 0; k != bitString.length(); k++)
		if (bitString[k] != ' ' && bitString[k] != '\t')
			return false;

	string bits;
	vector<unsigned short> result;

	for (size_t k = 0; k != bitString.length(); k++)
	{
		if (bitString[k] == ' ') bits += '0';
		else if (bitString[k] == '\t') bits += '1';
	}

	unsigned int numChunks = bits.length() / 16;
	for (unsigned int k = 0; k != numChunks; k++)
	{
		string tempNum;
		for (unsigned int i = k * 16; i != (k * 16) + 16; i++)
		{
			tempNum += bits[i];
		}
		unsigned short number;
		convertBitStringToNumber(tempNum, number);
		result.push_back(number);
	}

	numbers = result;
	return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}