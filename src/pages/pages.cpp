#include "pages.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "../include/logger.hpp"

using namespace ukrnet;

// default constructor
Pages::Pages(std::string data_file_path)
	: _data_file_path(data_file_path)
	, _data_file(0)
{
	// try to open data file
	_data_file = open(_data_file_path.data(), O_RDWR | O_CREAT, (mode_t)0600);
	if (_data_file < 0)
		logger().err("pages", "cannot open data file", _data_file_path, curr_errno_msg());
}

// destructor: closes file descriptor
Pages::~Pages()
{
	close(_data_file);
}

// generate test file
void Pages::GenerateTestFile()
{
	int value(150);
	size_t new_length = (value / 7) * sizeof(int);

	// extend file at one item
	int error = ftruncate(_data_file, new_length);
	if (error < 0)
	{
		logger().err("pages", "cannot truncate file", curr_errno_msg());
		return;
	}

	Map map = OpenMap();
	int *current = map.ptr;
	while ((value > 0) && (std::distance(current, map.ptr_end) > 0))
	{
		*current = value;
		++current;
		value -= 7;
	}
	CloseMap(map);
}

// returns item exists in file
bool Pages::ItemExists(int item)
{
	// map all file
	Map map = OpenMap();
	// find item
	int *item_ptr = FindItem(map, item);
	bool result = (item_ptr != nullptr);
	// close map and exit
	CloseMap(map);
	return result;
}

// get data of specified page
std::vector<int> Pages::GetPage(size_t page_idx, size_t page_size)
{
	// map current page
	Map map = OpenMap();
	int *page_begin = map.ptr + page_idx * page_size;
	int *page_end = std::min(page_begin + page_size, map.ptr_end);
	std::vector<int> page_data;
	// copy page data
	if (page_begin < page_end)
		page_data.assign(page_begin, page_end);
	// close map and exit
	CloseMap(map);
	return page_data;
}

// get page index of specified item
size_t Pages::GetItemPage(int item, size_t page_size)
{
	// map all file
	Map map = OpenMap();
	// find and remember item
	int *item_ptr = FindItem(map, item);
	size_t result = (item_ptr != nullptr)
		? std::distance(map.ptr, item_ptr) / page_size
		: -1;
	// close map and exit
	CloseMap(map);
	return result;
}

// add specified item
bool Pages::AddItem(int item)
{
	// exit if item already exists
	if (ItemExists(item))
	{
		logger().wrn("pages", "item already exists", item);
		return false;
	}

	// extend file at one item
	ftruncate(_data_file, (GetItemCount() + 1) * sizeof(int));

	// map all file
	Map map = OpenMap();
	// find insert hint and remember some data
	int *item_hint = FindInsertHint(map, item);
	// move [hint, end-1) to [hint+1, end)
	std::copy(item_hint, std::prev(map.ptr_end), std::next(item_hint));
	// setup hint into item
	*item_hint = item;
	CloseMap(map);
	return true;
}

// erase specified item
bool Pages::EraseItem(int item)
{
	// map all file
	Map map = OpenMap();
	// find item and remember some data
	int *item_ptr = FindItem(map, item);
	if (item_ptr == nullptr)
	{
		// item not found
		CloseMap(map);
		logger().wrn("pages", "item not exists", item);
		return false;
	}
	// move [item+1, end) to [item, end-1)
	std::copy(std::next(item_ptr), map.ptr_end, item_ptr);
	CloseMap(map);

	// shrink file at one item
	ftruncate(_data_file, (GetItemCount() - 1) * sizeof(int));

	return true;
}

// returns items count
size_t Pages::GetItemCount()
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

	return file_info.st_size / sizeof(int);
}

// returns pages count
size_t Pages::GetPageCount(size_t page_size)
{
	size_t count = GetItemCount();
	return (count / page_size) + ((count % page_size) > 0 ? 1 : 0);
}

// open mmap for all file
auto Pages::OpenMap() -> Map
{
	size_t size = GetItemCount();
	// exit if size is zero
	if (size == 0)
		return Map();
	// try to map data file in memory
	int *ptr = (int *)mmap(0, size * sizeof(int), 
		PROT_READ | PROT_WRITE, MAP_SHARED, _data_file, 0);
	if (ptr == MAP_FAILED)
	{
		logger().err("pages", "cannot map data file", curr_errno_msg());
		return Map();
	}
	return Map(ptr, size);
}

// find item pointer
int *Pages::FindItem(Map map, int item)
{
	int *item_ptr = std::lower_bound(map.ptr, map.ptr_end, item, std::greater<int>());
	if ((item_ptr != map.ptr_end) && (*item_ptr == item))
		return item_ptr;
	else
		return nullptr;
}

// find item insert hint
int *Pages::FindInsertHint(Map map, int item)
{
	int *item_ptr = std::lower_bound(map.ptr, map.ptr_end, item, std::greater<int>());
	return item_ptr;
}

// close mmap
void Pages::CloseMap(Map map)
{
	if (map.ptr == nullptr)
	{
		logger().err("pages", "data file not mapped");
		return;
	}
	// try to sync data to file
	int error = msync(map.ptr, map.size * sizeof(int), MS_SYNC);
	if (error < 0)
	{
		logger().err("pages", "cannot sync data file", curr_errno_msg());
		return;
	}
	// try to unmap data file
	error = munmap(map.ptr, map.size * sizeof(int));
	if (error < 0)
		logger().err("pages", "cannot unmap data file", curr_errno_msg());
}
