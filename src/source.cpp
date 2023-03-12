#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <string>
#include <mutex>
#include <queue>
#include <atomic>

int total_lines(0);
std::queue<std::filesystem::path> file_list;
std::mutex lines_mutex; // protects total_lines
std::mutex list_mutex; // protects file_list

void count_lines()
{
	while (true)
	{
		std::filesystem::path file_name;
		{
			const std::lock_guard<std::mutex> lock(list_mutex);
			if (file_list.empty()) return; // quits thread
			file_name = file_list.front();
			file_list.pop();
		}

		int lines = 0;
		std::ifstream in_file(file_name);
		std::string temp;
		while (std::getline(in_file, temp)) ++lines;
		{
			const std::lock_guard<std::mutex> lock(lines_mutex);
			total_lines += lines;
		}
		//std::cout << file_name << std::endl;
		//std::cout << lines << std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Directory path not specified" << std::endl;
		return 1;
	}
	else if (argc > 2)
	{
		std::cout << "Too many arguments" << std::endl;
		return 1;
	}

	//std::cout << argv[1] << std::endl;
	//std::cout << argc << std::endl;
	if (std::filesystem::is_directory(argv[1]))
	{
		//std::cout << "Direcotry path correct" << std::endl;
		for (const auto& file : std::filesystem::directory_iterator(argv[1]))
		{
			const std::lock_guard<std::mutex> lock(list_mutex); // not really needed as of now 
			file_list.push(file.path());
		}

		std::vector<std::thread> threads;
		int thread_count = std::thread::hardware_concurrency();
		//std::cout << thread_count << " concurrent threads are supported" << std::endl;
		for (int i = 0; i < thread_count; ++i)
		{
			threads.push_back(std::thread(count_lines));
		}
		for (auto& th : threads) th.join(); // wait for threads to finish
		std::cout << total_lines << std::endl;
	}
	else
	{
		std::cout << "Incorrect directory path" << std::endl;
		return 1;
	}
	
	return 0;
}