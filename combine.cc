#include <stdshit.h>
#include <time.h>
#include "cppblock.cc"
#include "sha1.h"

const char progName[] = "combine";
cppMacroLst mList;

void buildDate(Bstr& fpOut)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer,80,"%c",timeinfo);
	fpOut.fmtcat("// BuildDate: %s\n", buffer);
}

xarray<cParseBlock_t> processFile(
	Bstr& fpOut, cch* parent, cch* name, bool);
void processBlock(Bstr& fpOut, cch* parent, 
	cch* name, cParseBlock_t& block)
{
	for(int i = 0; i < block.len; i++) {
		auto& blk = block[i];
		
		int type = blk.cppType();
		
		switch(type) {
		case CPP_DEFINE: case CPP_UNDEF:
			mList.insert(blk); break;
		
		
		
		case CPP_IFDEF: case CPP_IFNDEF: {
			int eval = mList.evaluate(blk);
			if(eval >= 0) block.branch(i, eval);
			break; }
			
		case CPP_INCLUDE: {
			char* arg = blk.arg();
			if(arg[0] == '<') { if(!mList.insert(blk)) {
				blk.kill(0); } continue; }
			if(arg[0] != '"') continue; arg++;
			char* end = strchr(arg, '"'); if(!end) 
			fatalError("bad include name");	*end = 0;
			blk.kill(0); blk.init(processFile(
				fpOut, name, arg, false)); continue; }
		}
		
		if(CPP_RNG_IF(block[i].type)) {
		
			cppMacroLst mOrg = mList;
			cppMacroLst mPrev;
			
			while(1) {
			auto& blk2 = block[i];
			processBlock(fpOut, parent, name, blk2);
			if(mPrev.dataPtr) mList.merge(mPrev);
			if(blk2.type == CPP_ENDIF) { 
				mList.merge(mOrg); break; }
			i++; if(blk2.type == CPP_ELSE) break;
			mPrev.Free(); mPrev.init(mList);
			mList.xCopy(mOrg);
			}
			
		} else {
			processBlock(fpOut, parent, name, blk);
		}
	}
}

xarray<cParseBlock_t> processFile(Bstr& fpOut,
	cch* parent, cch* name, bool keepGuard)
{
	// load and parse file
	if(parent != name)
	name = replName(parent, name);
	char* file = loadText(name).data;
	if(!file) fatalError("load error: %s\n", name);
	cParseBlock_t block; block.parse(file);

	// detect guard band
	if(!keepGuard && (block.len == 2)
	&&(block[0].cppType() == CPP_IFNDEF)
	&&(block[1].cppType() == CPP_ENDIF)
	&&(block[0].cppType(0) == CPP_DEFINE)) { 
		if(mList.insert(block[0][0]) == 0) {
			free(file); return {0,0}; }
		block.branch(0,1); block[0].kill(0,0);
	}
	
	processBlock(fpOut, parent, name, block);
	return block.release();
}

void outputBlocks(Bstr& fpOut, xarray<cParseBlock_t> block)
{
	for(auto& blk : block) { {
	if(blk.str) { 
		fpOut.fmtcat("%s\n", blk.str); }
		outputBlocks(fpOut, blk); 
	}}
}

void saveFile_OnChange(cch* dst, cstr str)
{
	// because cmake is broken
	auto data = loadFile(dst);
	if((data.len != str.slen)
	||(memcmp(data.data+96, str.data+96, str.slen-96)))
		saveFile(dst, str.data, str.slen);
}

void processFile(const char* src, 
	const char* dst, bool isSourceFile)
{
	char* progDir = getProgramDir();
	src = pathCat(progDir, src);
	dst = pathCat(progDir, dst);

	Bstr fpOut;
	fpOut.fmtcat("// stdshit.h: Single file version\n");
	fpOut.fmtcat("// DeadFish Shitware 2013-2014\n");
	buildDate(fpOut);
	
	auto blocks = processFile(fpOut, src, src, true);
	outputBlocks(fpOut, blocks);
	saveFile_OnChange(dst, fpOut);
}

int main(int argc, char* argv[])
{
	// option tables
	static const char* optName[] = {"noasm", "sect", 
		"dele", "endi", "comp", "utf8", "xfmt" };
	static const char* optIfdef[] = {"x86bits", "no_lowerSect", "no_delagates",
		"no_endian", "no_compar", "no_utf816", "no_xstrfmt" };
	static bool optState[7];
	
	// initialize options
	for(int i = 1; i < argc; i++) {
	for(int j = 0; j < 7; j++) { if(!strcmp(argv[i],
		optName[j])) { optState[j] = true; break; }}}
	for(int j = 0; j < 7; j++) {
		mList.define(optIfdef[j], optState[j]); }
	
	processFile("src\\stdshit.h", "bin\\stdshit.h", false);
	//processFile("src\\stdshit.cc", "bin\\stdshit.cc", true);
}
