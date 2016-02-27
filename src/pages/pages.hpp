#pragma once
#include <string>
#include <vector>
#include "../include/stream-helper.hpp"

namespace ukrnet
{
	class Pages
	{
	public:
		typedef std::vector<int> vecData;
		typedef bin_wrap<int> item;

		struct Map
		{
			int *ptr;
			size_t size;
			Map()
				: ptr(nullptr)
				, size(0)
			{}
			Map(int *ptr, size_t size)
				: ptr(ptr), size(size) 
			{}
		};

	public:
		Pages(std::string data_file_path);
		~Pages();

	public:
		vecData GetPage(size_t page_idx, size_t page_size);
		int GetItemPage(int item, size_t page_size);
		void AddItem(int item);
		void EraseItem(int item);
		size_t GetItemCount();
		size_t GetPageCount(size_t page_size);

	private:
		Map OpenMap(size_t page_idx, size_t page_size, size_t page_count = 1);
		void CloseMap(Map map);

	private:
		std::string _data_file_path;
		int _data_file;

	};
}