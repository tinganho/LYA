
#define CURL_STATICLIB
#include "configurations.h"
#include <stdio.h>
#include <fstream>
#include <curl/curl.h>
#include <utils.h>
#include <contrib/minizip/unzip.h>
#include <string>

namespace fs = boost::filesystem;
using namespace Lya::lib::utils;

const char *cldr_folder = PROJECT_DIR "src/cldr/";

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void unzip(const char* file) {
	unzFile zip = unzOpen(file);
	if (zip) {
		if (unzGoToFirstFile(zip) == UNZ_OK) {
			do {
				if (unzOpenCurrentFile(zip) == UNZ_OK) {
					unz_file_info file_info;
					memset(&file_info, 0, sizeof(unz_file_info));
					if (unzGetCurrentFileInfo(zip, &file_info, NULL, 0, NULL, 0, NULL, 0) == UNZ_OK) {
						char *filename = (char *)malloc(file_info.size_filename + 1);
						unzGetCurrentFileInfo(zip, &file_info, filename, file_info.size_filename + 1, NULL, 0, NULL, 0);
						filename[file_info.size_filename] = '\0';
						char *abs_filename = new char[strlen(cldr_folder) + strlen(filename) + 1];
						strcpy(abs_filename, cldr_folder);
						strcat(abs_filename, filename);
						string folder = folder_path(abs_filename);
						if (!path_exists(folder)) {
							create_folder(abs_filename);
							free(filename);
							continue;
						}
						FILE *out = fopen(abs_filename, "wb");
						unsigned char buffer[4096];
						int read_bytes;
						while ((read_bytes = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0) {
							fwrite(buffer, read_bytes, 1, out);
						}
						fclose(out);
						free(filename);
						delete[] abs_filename;
					}

					unzCloseCurrentFile(zip);
				}
			} while (unzGoToNextFile(zip) == UNZ_OK);
		}
		unzClose(zip);
	}
}

int main(void) {
	CURL *curl;
	FILE *fp;
	CURLcode res;
	remove_folder(cldr_folder);
	create_folder(cldr_folder);
	curl = curl_easy_init();
	char f[512];
	strcpy(f, strcat(const_cast<char*>(boost::filesystem::temp_directory_path().c_str()), "LYA/cldr.zip"));
	create_folder(folder_path(f));
	cout << f << endl;
	if (curl) {
//		fp = fopen(strcat(const_cast<char*>(boost::filesystem::temp_directory_path().c_str()), "LYA/cldr.zip"), "w");
//		curl_easy_setopt(curl, CURLOPT_URL, "https://unicode.org/Public/cldr/32/cldr-common-32.zip");
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
//		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
//		curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
//		res = curl_easy_perform(curl);
//		curl_easy_cleanup(curl);
//		fclose(fp);
		unzip(f);
	}
	return 0;
}