#pragma once

#include <ctime>
#include <unistd.h>
#include <string>
#include <cstring>

namespace utils {

class ChunkedFile {
	static const char FILE_SEP = '/';
	static constexpr long DEFAULT_SPLIT_TIME = 60; // 1 min

	const std::string m_root;
	const std::string m_file_pref;
	const std::string m_file_ext;
	FILE* m_file;
	long m_time_split_sec;
	time_t m_time_last_update;

public:

	ChunkedFile(const char* root_dir, const char* file_pref, const char* file_ext) :
		m_root(root_dir), m_file_pref(file_pref), m_file_ext(file_ext), m_file(nullptr), m_time_split_sec(
		DEFAULT_SPLIT_TIME), m_time_last_update(0) {}

	ChunkedFile(const ChunkedFile&) = delete;
	ChunkedFile& operator=(const ChunkedFile&) = delete;

	ChunkedFile(ChunkedFile&&) = delete;
	ChunkedFile& operator=(ChunkedFile&&) = delete;

	~ChunkedFile() {
		close();
	}

	void set_split_time(long time_split_sec) noexcept {
		m_time_split_sec = time_split_sec;
	}

	int open() {
		if(m_file)
			return -1;

		time_t cur_time = time(nullptr);
		struct tm local_time;
		localtime_r(&cur_time, &local_time);

		std::string current_dir = dir_name(local_time);
		std::string current_file = file_name(local_time);

		if(create_dir(current_dir) == 0) {
			m_file = open_file(current_file);
			if(m_file) {
				m_time_last_update = cur_time;
			}
		}

		return m_file == nullptr;
	}

	void close() {
		if(m_file)
			fclose(m_file);
		m_file = nullptr;
	}

	bool update() {
		time_t time_current = time(NULL);
		if(time_current - m_time_last_update >= m_time_split_sec || not same_day(time_current, m_time_last_update)) {
			close();
			return open() == 0;
		}
		return false;
	}

	operator FILE*() const {
		return m_file;
	}

	FILE* get() const {
		return m_file;
	}

private:

	std::string file_name(struct tm time) const noexcept {
		return dir_name(time) + FILE_SEP + m_file_pref + format_data_time(time) + "." + m_file_ext;
	}

	std::string dir_name(struct tm time) const noexcept {
		return m_root + FILE_SEP + format_data(time);
	}

	static int create_dir(std::string dir_name) noexcept {
		std::string cmd = "mkdir -p " + dir_name;
		return system(cmd.c_str());
	}

	static FILE* open_file(std::string file_name) noexcept {
		return fopen(file_name.c_str(), "a");
	}

	static std::string format_data(struct tm time) noexcept {
		constexpr unsigned BUF_SIZE = 64;
		char buffer[BUF_SIZE];

		snprintf(buffer, BUF_SIZE, "%04d-%02d-%02d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
		return std::string(buffer);
	}

	static std::string format_data_time(struct tm time) noexcept {
		constexpr unsigned BUF_SIZE = 64;
		char buffer[BUF_SIZE];

		snprintf(buffer, BUF_SIZE, "%04d-%02d-%02d_%02d-%02d-%02d",
		         time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
		         time.tm_hour, time.tm_min, time.tm_sec
		);
		return std::string(buffer);
	}

	static bool same_day(time_t first, time_t second) {
		struct tm fr;
		struct tm sc;

		localtime_r(&first, &fr);
		localtime_r(&second, &sc);

		return fr.tm_year == sc.tm_year && fr.tm_yday == sc.tm_yday;
	}

};

}; // namespace utils

#endif /* CHUNKED_FILE_H */

