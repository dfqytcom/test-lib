#include "common.h"
#include <Windows.h>
#include <sstream>

/*
	char* to jstring
	
*/
jstring charsTojstring(JNIEnv* env, const char* str)
{//gb2312  to  utf8/16
	jstring rtn = 0;
	int slen = strlen(str);
	unsigned short * buffer = 0;
	if (slen == 0)
		rtn = (env)->NewStringUTF(str);
	else
	{
		int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, NULL, 0);
		buffer = (unsigned short *)malloc(length * 2 + 1);
		if (MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str, slen, (LPWSTR)buffer, length) >0)
			rtn = (env)->NewString((jchar*)buffer, length);
	}
	if (buffer){

		free(buffer);
		
	}
	
	return rtn;
}
/*
	jstring to char*
*/
char* jstringToChars(JNIEnv *env, jstring jstr)
{ //UTF8/16 to gb2312
	
	int length = (env)->GetStringLength(jstr);
	const jchar* jcstr = (env)->GetStringChars(jstr, 0);

	int clen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)jcstr, length, NULL, 0, NULL, NULL);

	char* rtn = (char*)malloc(clen);
	//更正。作者原来用的是(char*)malloc( length*2+1 )，当java字符串中同时包含汉字和英文字母时，所需缓冲区大小并不是2倍关系。
	int size = 0;
	size = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)jcstr, length, rtn, clen, NULL, NULL);
	if (size <= 0)
		return NULL;
	//释放新建的jstring
	//cout << "释放JNIenv" << endl;
	(env)->ReleaseStringChars(jstr, jcstr);
	
	//cout << "释放JNIenv成功了 ^^ " << endl;

	rtn[size] = 0;
	return rtn;
}
/*
turn jstring to string
*/
string jstringToString(JNIEnv * env, jstring src)
{
	//jstring to char *
	const char * ch = jstringToChars(env, src);

	//char* to string
	string result = string(ch);

	return result;
}
/*
turn string to jstring
*/
jstring stringToJstring(JNIEnv *env, string src)
{
	//string to char*
	const char* ch = src.c_str();
	//char* to jstring
	jstring result = charsTojstring(env, ch);

	return result;
}

/*
turn string to int
*/
int stringToInt(string src){

	int result = 0;
	stringstream ss;
	ss << src;
	ss >> result;
	return result;

}

/*
turn int to string
*/
string intToString(const int &int_temp)
{
	stringstream stream;
	string result;
	stream << int_temp;
	result = stream.str();   //此处也可以用 stream>>string_temp  
	return result;
}