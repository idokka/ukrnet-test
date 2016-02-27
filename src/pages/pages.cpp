#include "pages.hpp"
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "../include/logger.hpp"

using namespace ukrnet;


Pages::Pages(std::string data_file_path)
	: _data_file_path(data_file_path)
	, _data_file(0)
{
	// try to open data file
	int _data_file = open(_data_file_path.data(), O_RDONLY, (mode_t)0600);
	if (_data_file < 0)
		logger().err("pages", "cannot open data file", _data_file_path, curr_errno_msg());
}

~Pages::Pages()
{
	close(_data_file);
}

auto Pages::GetPage(size_t page_idx, size_t page_size) -> vecData
{

}

int Pages::GetItemPage(int item, size_t page_size)
{

}

void Pages::AddItem(int item)
{

}

void Pages::EraseItem(int item)
{

}

size_t GetItemCount()
{
	if (_data_file < 0)
		return 0;
	struct stat file_info = {0};

	int error = fstat(_data_file, &file_info);
	if (error < 0)
	{
		logger().err("pages", "Error getting the file size", curr_errno_msg());
		return 0;
	}

	return file_info.st_size;
}

size_t GetPageCount(size_t page_size)
{
	size_t count = GetItemCount();
	return (count / page_size) + ((count % page_size) > 0 ? 1 : 0);
}

Map Pages::OpenMap(size_t page_idx, size_t page_size, size_t page_count = 1)
{
	// try to map data file in memory
	size_t size = page_count * page_size;
	size_t offset = page_idx * page_size;
	int *ptr = (int *)mmap(0, size, PROT_READ, MAP_SHARED, _data_file, offset);
	if (ptr == MAP_FAILED)
	{
		logger().err("pages", "cannot map data file", curr_errno_msg());
		return Map;
	}
	return Map(ptr, size);
}

void Pages::CloseMap(Map map)
{
	// try to unmap data file
	int error = munmap(map.ptr, map.size);
	if (error < 0)
		logger().err("pages", "cannot unmap data file", curr_errno_msg());
}
