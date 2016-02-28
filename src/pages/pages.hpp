#pragma once
#include <string>
#include <vector>
#include "../include/stream-helper.hpp"

namespace ukrnet
{
	// page manager for task 2.2
	class Pages
	{
	public:
		// summary struct for mmap
		struct Map
		{
			// ptr to begin of file
			int *ptr;
			// ptr to end of file
			int *ptr_end;
			// size of file in items
			size_t size;
			// default empty constructor
			Map()
				: ptr(nullptr)
				, ptr_end(nullptr)
				, size(0)
			{}
			// default constructor
			Map(int *ptr, size_t size)
				: ptr(ptr)
				, size(size)
				, ptr_end(ptr + size)
			{}
		};

	public:
		// default constructor
		Pages(std::string data_file_path);
		// destructor: closes file descriptor
		~Pages();
		// generate test file
		void GenerateTestFile();

	public:
		// returns item exists in file
		bool ItemExists(int item);
		// get data of specified page
		std::vector<int> GetPage(size_t page_idx, size_t page_size);
		// get page index of specified item
		size_t GetItemPage(int item, size_t page_size);
		// add specified item
		bool AddItem(int item);
		// erase specified item
		bool EraseItem(int item);
		// returns items count
		size_t GetItemCount();
		// returns pages count
		size_t GetPageCount(size_t page_size);

	private:
		// open mmap for all file
		Map OpenMap();
		// open mmap for specified page
		Map OpenMap(size_t page_idx, size_t page_size);
		// close mmap
		void CloseMap(Map map);
		// find item pointer
		// returns nullptr, if item not exists
		int *FindItem(Map map, int item);
		// find item insert hint
		int *FindInsertHint(Map map, int item);

	private:
		// data file path
		std::string _data_file_path;
		// data file descriptor
		int _data_file;

	};
}