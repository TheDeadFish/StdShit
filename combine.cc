#include "src\stdshit.h"
#include <time.h>
const char progName[] = "combine";

// cutdown modes
bool no_x86bits = false;
bool no_lowerSect = false;
bool no_delagates = false;
bool no_endian = false;
bool no_compar = false;
bool no_utf816 = false;
bool no_xstrfmt = false;


DEF_RETPAIR(checkOption_t, char*, str, bool, no);
checkOption_t checkOption(char* option) {
	if(char* str = strstr(option, "x86bits"))
		return checkOption_t(str, no_x86bits);
	if(char* str = strstr(option, "no_lowerSect"))
		return checkOption_t(str, no_lowerSect);
	if(char* str = strstr(option, "no_delagates"))
		return checkOption_t(str, no_delagates);
	if(char* str = strstr(option, "no_endian"))
		return checkOption_t(str, no_endian);
	if(char* str = strstr(option, "no_compar"))
		return checkOption_t(str, no_compar);
	if(char* str = strstr(option, "no_utf816"))
		return checkOption_t(str, no_utf816);
	if(char* str = strstr(option, "no_xstrfmt"))
		return checkOption_t(str, no_xstrfmt);
	return NULL; }
bool checkLine(char* line, int ifLevel) 
{
	struct Stack { int state;
		int curIfLevel; }; 
	static Stack stack[16];
	static int stackPos = -1;
	
	// check for enter condition
	if(!strncmp(line, "#ifndef "))
	if(auto result = checkOption(line+8)) {
		//printf(" ^%d, --------- %s ------------\n", ifLevel, result);
		auto& slot = stack[++stackPos];
		slot.state = result.no; 
		slot.curIfLevel = ifLevel;
		return false; }
	if(stackPos < 0) return true;
		
	// check for exit condition
	auto& slot = stack[stackPos];
	if(slot.curIfLevel == ifLevel) {
		//printf("@ ^%d %s\n", ifLevel, line);
	
	
		if(!strncmp(line, "#else")) {
			slot.state ^= 1; return false; }
		if(!strncmp(line, "#endif")) {
			stackPos--; return false; }
	} return !(slot.state & 1);
}

// include file dedup
char** includeList;
int includeCount;
bool checkInclude(char* line)
{
	if(strncmp(line, "#include <"))
		return true;
	for(int i = 0; i < includeCount; i++)
	  if(!stricmp(includeList[i], line)) {
		//printf("%s\n", line);
		return false; }
	xNextAlloc(includeList, includeCount) = 
		xstrdup(line);
	return true;
}

struct CParse
{
	enum ParseMode {
		NormalMode,
		InComment, InMacro,
		InString1, InString2
	} parseMode;
	bool prevMacro;
	int ifLevel = 0;
	
	CParse() : parseMode(NormalMode) {}
	bool line(char* line);
	void normalMode(char* base, char*& line);
	bool isOperator(char ch);
};

bool CParse::isOperator(char ch)
{
	char str[2] = {ch, 0};
	if(ch == 0) return true;
	return strpbrk(str, "=+-*/%!<>&|~^[](),?:\\\"\'{};");
}

inline 
void CParse::normalMode(char* base, char*& line)
{
	switch(*line)
	{
	case ' ':
	case '\t':
		if((((line-1 < base)
		||(isOperator(line[-1]))
		||(isOperator(line[1]))))
		&&(((line-base) != 9)
		||(!strScmp(base, "extern\"C\""))))
		{
			strcpy(line, line+1);
			line = line-1;
		}
		break;
	case '\'':
		parseMode = InString1;
		break;
	case '"':
		parseMode = InString2;
		break;
	case '#':
		parseMode = InMacro;
		break;
	default:
		if(!strncmp(line, "/*")) 
		{
			parseMode = InComment;
			strcpy(line, line+2);
			line = line-1;
		}
		ei(!strncmp(line, "//"))
		{
			line[0] = '\0';
			line[1] = '\0';
		}
	}
}

bool CParse::line(char* line)
{
	bool curMacro = false;
	char* lineBase = line;
	removeCrap(line);
	for(; *line; line++)
	{
		switch(parseMode) 
		{
		case NormalMode:
			normalMode(lineBase, line);
			break;
		case InComment:
			if(!strncmp(line, "*/")) {
				parseMode = NormalMode;
				strcpy(line, line+2); }
			else 
				strcpy(line, line+1);
			line = line-1;
			break;
		case InMacro:
			curMacro = true;
			if((line[1] == '\0')
			&&(line[0] != '\\'))
				parseMode = NormalMode;
			break;
		case InString1:
			if(line[0] == '\'')
				parseMode = NormalMode;
			break;
		case InString2:
			if(line[0] == '"')
				parseMode = NormalMode;		
			break;
		}
	}
	
	// newLine ?
	bool newLine = curMacro || prevMacro ||
		lineBase[strspn(lineBase, "{};")];
	prevMacro = curMacro;
	if(!checkInclude(lineBase))
		lineBase[0] = '\0';
	
	// handle ifdef
	if(strnicmp(lineBase, "#if"))
		ifLevel += 1;
	bool skipLine = checkLine(lineBase, ifLevel);
	//printf("%d, %s\n", skipLine, lineBase);
	
	if(strnicmp(lineBase, "#endif"))
		ifLevel -= 1;
	if(skipLine == false)
		lineBase[0] = '\0';
	return newLine;
}

bool include(FILE* fpOut, char* line)
{
	#define WRITE_LINE() { \
		if(newLine == true) \
			fputs("\n", fpOut); \
		fputs(line, fpOut); } \

	strcpy(line, "src\\");
	strcpy(line+4, line+10);
	strchr(line, '"')[0] = '\0';
	FILE* fpIn = xfopen(line, "!r");
	
	int guardCheck = 0;
	int lastEndif = 0;
	CParse cParse;
	while(fgets(line, 512, fpIn))
	{
		bool newLine = cParse.line(line);
		if(strlen(line) == 0)
			continue;
		if(guardCheck == 0) { guardCheck = 1;
		if(!strncmp(line, "#ifndef"))
		{	guardCheck = 2; continue; }}
		if(guardCheck == 2)
		{	guardCheck = -1; continue; }
		if(!strncmp(line, "#endif"))
			lastEndif = ftell(fpOut);
		WRITE_LINE();
	}
	if(guardCheck < 0) {
		printf("FUCK: %d\n", lastEndif);
		fseek(fpOut, lastEndif, SEEK_SET); }
	fclose(fpIn);
}

void buildDate(FILE* fpOut)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer,80,"%c",timeinfo);
	fprintf(fpOut, "// BuildDate: %s\n", buffer);
}

void processFile(const char* src, const char* dst,
	bool isSourceFile)
{
	CParse cParse;
	char line[512];
	FILE* fpIn = xfopen(src, "!r");
	FILE* fpOut = xfopen(dst, "!wb");
	fprintf(fpOut, "// stdshit.h: Single file version\n");
	fprintf(fpOut, "// DeadFish Shitware 2013-2014\n");
	buildDate(fpOut);

	while(fgets(line, 512, fpIn))
	{
		bool newLine = cParse.line(line);
		if(strlen(line) == 0)
			continue;
		if((!strncmp(line, "#include \""))
		&&(strstr(line, "stdshit.h") == NULL)
		&&((!isSourceFile)||(strstr(line, ".cpp"))))
			include(fpOut, line);
		else
			WRITE_LINE();
	}
	fclose(fpIn);
	fclose(fpOut);
}

int main(int argc, char* argv[])
{
	// get build options
	for(int i = 1; i < argc; i++) {
		if(strstr(argv[i], "noasm"))
			no_x86bits = true;
		if(strstr(argv[i], "sect"))
			no_lowerSect = true;
		if(strstr(argv[i], "dele"))
			no_delagates = true;	
		if(strstr(argv[i], "endi"))
			no_endian = true;	
		if(strstr(argv[i], "comp"))
			no_compar = true;	
		if(strstr(argv[i], "utf8"))
			no_utf816 = true;	
		if(strstr(argv[i], "xfmt"))
			no_xstrfmt = true;
	}
	
	processFile("src\\stdshit.h", "bin\\stdshit.h", false);
	//processFile("src\\stdshit.cc", "bin\\stdshit.cc", true);
}
