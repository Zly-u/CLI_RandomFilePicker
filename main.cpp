#include <array>
#include <filesystem>
#include <iostream>
#include <print>
#include <random>

#include <fcntl.h>
#include <map>
#include <ranges>
#include <vcruntime_startup.h>

#include <shlobj_core.h>
#include "Windows.h"

#undef max
#undef min

constexpr void trim(std::wstring& s) {
	constexpr auto not_space = [](unsigned char c){ return !std::isspace(c); };

	// erase the spaces at the end
	s.erase(
		std::ranges::find_if(s | std::views::reverse, not_space).base(),
		s.end()
	);

	// erase the spaces at the start
	s.erase(
		s.begin(),
		std::ranges::find_if(s, not_space)
	);
}

int wmain(int argc, wchar_t** argv) {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	
	const std::array<std::string, 4> extensions = {
		".mp4", ".mov", ".wmv"
	};

	std::vector<std::wstring> list_of_paths;
	
	size_t num_of_elems = 0;
	if (argc >= 3) {
		try {
			num_of_elems = std::stoi(argv[1]);
		} catch(...) {}

		for(size_t ind = 2; ind < argc; ind++) {
			if(!std::filesystem::exists(argv[ind])) {
				continue;
			}
			list_of_paths.emplace_back(argv[ind]);
		}

		if (list_of_paths.size() - argc - 2 != 0) {
			std::println("[WARNING] Some of the passed paths were invalid.\nProcessing the ones that are fine.");
		}
	} else {
		std::println("=============================================================");
		std::println("You can use this program as a command line tool, e.g.:");
		std::println("`> app.exe <AmountOfFilesToPick> <Path1> [Path2] [Path3] ...`");
		std::println("Supports any amount of passed paths.");
		std::println("=============================================================\n");
		
		std::println("Enter a number of files you want to randomly pick [1..32]");
		
arg_1: // I see you cringe :)
		std::print("Input: ");
		std::string input_number;
		std::cin >> input_number;
		std::println("");

		try {
			num_of_elems = std::stoi(input_number);
			if (num_of_elems > 32 || num_of_elems == 0) {
				goto arg_1;
			}
		} catch(...) {
			goto arg_1;
		}
		
		std::println("Enter a list of paths to go through, separate each path with a `;`.");
		std::wstring input_paths;
		std::print("Input: ");
		std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
		std::getline(std::wcin, input_paths);
		std::println("");

		std::wstringstream ss(input_paths);
		std::wstring token;
		uint32_t found = 0;
		while (std::getline(ss, token, L';')) {
			found++;
			
			trim(token);
			if (!std::filesystem::exists(token)) {
				std::wcout << "[ERROR] Invalid path: " << token << '\n';
				continue;
			}
			
			list_of_paths.push_back(token);
		}
		
		if (list_of_paths.size() - found != 0) {
			std::println("[WARNING] Some of the passed paths were invalid.\nProcessing the ones that are fine.");
		}
	}
	
	if (list_of_paths.empty()) {
		std::println("[ERROR] The passed paths are invalid/don't exist.");
		return -1;
	}
	
	std::vector<std::filesystem::path> file_paths;
	for(auto& path : list_of_paths) {
		if (!std::filesystem::exists(path)) {
			continue;
		}
		auto iter = std::filesystem::recursive_directory_iterator(path);
		for(const std::filesystem::directory_entry& p : iter) {
			if (p.is_directory()) { continue; }
			if (!p.exists()) { continue; }
			if (!std::ranges::contains(extensions, p.path().extension().string())) { continue; }
			
			file_paths.emplace_back(p.path());
		}
	}
	
	//std::filesystem::directory_options::skip_permission_denied
	if (num_of_elems == 0) {
		num_of_elems = 1;
	}
	num_of_elems = std::min(num_of_elems, file_paths.size());
	
	std::vector<std::filesystem::path> found_files;
	std::ranges::shuffle(
		file_paths,
		std::mt19937{std::random_device{}()}
	);
	std::ranges::sample(
		file_paths,
        std::back_inserter(found_files),
        num_of_elems,
        std::mt19937{std::random_device{}()}
	);
	
	std::wcout.imbue(std::locale(".UTF-8"));
	
	std::map<std::filesystem::path, std::vector<std::filesystem::path>> dirs_of_files;

	for(std::filesystem::path& path : found_files) {
		if(!dirs_of_files.contains(path.parent_path())) {
			dirs_of_files[path.parent_path()] = std::vector<std::filesystem::path>();
		} 
		dirs_of_files.at(path.parent_path()).emplace_back(path);
	}
	
	if(CoInitialize(nullptr) != S_OK) { return -1; }
	for(auto& [dir, files] : dirs_of_files) {
		std::wcout << "dir: " << dir.wstring() << "\n";
		
		LPCITEMIDLIST* items = new LPCITEMIDLIST[files.size()];
		for(size_t index = 0; index < files.size(); index++) {
			std::wcout << "\tfile: " << files[index] << "\n";
			items[index] = ILCreateFromPathW(files[index].c_str());
		}

		std::wstring a = dir.wstring();
		const PIDLIST_ABSOLUTE w_dir = ILCreateFromPathW(a.c_str());
		if(!w_dir) { continue; }
		if(SHOpenFolderAndSelectItems(w_dir, files.size(), items, 0) != S_OK) { continue; }
		ILFree(w_dir);
		
		delete[] items;
	}

	return 0;
}







