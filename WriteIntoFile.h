#pragma once
#include <string>
#include <iostream>
#include <cstring>

using namespace std;

class WriteIntoFile
{
private:
	string aName = "";

public:
    WriteIntoFile() {}
    WriteIntoFile(const char* name)
		: aName(name ? name : "")
	{}
	void Write(const unsigned char* text) {
		if (!this->aName.empty())
		{
			FILE* f = fopen(this->aName.c_str(), "wb");

			if (f)
			{
				int length = (int)strlen((const char*)text);
				fwrite(text, length, 1, f);
				fclose(f);
			}
		}
		else
		{
			cout << text << endl;
		}
	}

};

