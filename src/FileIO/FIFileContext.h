#ifndef FIFILECONTEXT_H
#define FIFILECONTEXT_H
#pragma once

#include <Foundation/Object.h>
#include <string>
#ifdef _WIN32
#include <WinBasis/WinBasis.h>
#endif
namespace Spr{;

class FIFileContext{
public:
	struct FileInfo{
		~FileInfo();
		std::string name;	///<	ファイル名
		int line;			///<	行番号
		const char* start;	///<	メモリマップされたファイルの先頭
		const char* end;	///<	メモリマップされたファイルの終端
#ifdef _WIN32
		HANDLE hFile, hFileMap;
#endif

		FileInfo():line(0),start(NULL), end(NULL){}
		bool Map(std::string fn);
		void Unmap();
	};
	UTStack<FileInfo> fileInfo;
	
	typedef UTStack<ObjectIf*> IfStack;
	///	現在ロード中のオブジェクト
	IfStack objects;
	///	コンテナが必要なばあい
	IfStack container;
	///	
	UTStack<void*> primitives;

	FIFileContext(){
	}
	ObjectIf* Create(const IfInfo* ifInfo, const void* desc);
};


}

#endif
