#include "UpdateChecker.h"
#include "VersionParser.h"
#include <set>


bool isVersionNewer(const std::string& currentVersion, const std::set<std::string>& versions)
{
	auto newestVersion = currentVersion;
	auto changed = false;
	for (const auto& version : versions)
	{
		const auto newestDotPos = newestVersion.find('.');
		const auto versionDotPos = version.find('.');
		
		if (std::stoi(version) > std::stoi(newestVersion))
		{
			newestVersion = version;
			changed = true;
			continue;
		}
		else if (std::stoi(version) < std::stoi(newestVersion))
			continue;

		// if parts before the dot are equal, compare the rest
		auto newestRightSide = newestDotPos == std::string::npos ? "" : newestVersion.substr(newestDotPos + 1);
		auto versionRightSide = versionDotPos == std::string::npos ? "" : version.substr(versionDotPos + 1);
		if (version.size() > newestVersion.size())
		{
			bool setAfterLoop = true;
			if (newestDotPos != std::string::npos)
			{
				for (unsigned int i = 0; i < newestRightSide.size(); ++i)
				{
					if (toupper(versionRightSide[i]) > toupper(newestRightSide[i]))
					{
						newestVersion = version;
						changed = true;
						setAfterLoop = false;
						break;
					}
					if (toupper(versionRightSide[i]) < toupper(newestRightSide[i]))
					{
						setAfterLoop = false;
						break;
					}
				}
			}
			if (setAfterLoop && versionRightSide.substr(newestRightSide.size(), versionRightSide.size() - newestRightSide.size()).find_first_not_of('0') != std::string::npos)
			{
				newestVersion = version;
				changed = true;
			}
		}
		else // version shorter or same length as newestVersion
		{
			if (versionDotPos != std::string::npos)
			{
				for (unsigned int i = 0; i < versionRightSide.size(); ++i)
				{
					if (toupper(versionRightSide[i]) > toupper(newestRightSide[i]))
					{
						newestVersion = version;
						changed = true;
						break;
					}
					else if (toupper(versionRightSide[i]) < toupper(newestRightSide[i]))
						break;
				}
			}
		}
	}

	if (changed)
		return true;
	return false;
}


std::set<std::string> getAllNumberedVersions(std::string& json)
{
	std::set<std::string> versions;
	size_t nameStart;
	size_t nameEnd = 0;
	do
	{
		size_t pos;
		pos = json.find("\"name\"", nameEnd);
		if (pos != std::string::npos)
			nameStart = pos + 8; // release name starts here
		else
			break;

		if (nameStart < json.size())
			nameEnd = json.find('"', nameStart + 1);
		else
			break;

		if (nameEnd < json.size() && isdigit(json[nameStart]))
			versions.insert(json.substr(nameStart, nameEnd - nameStart));
	} while (nameEnd != std::string::npos && nameEnd < json.size());

	return versions;
}


//
//  libcurl variables for error strings and returned data

static char errorBuffer[CURL_ERROR_SIZE];
static std::string buffer;

//
//  libcurl write callback function
//

static int writer(char* data, size_t size, size_t nmemb, std::string* writerData)
{
	if (!writerData)
		return 0;

	writerData->append(data, size * nmemb);

	return size * nmemb;
}

//
//  libcurl connection initialization
//

static bool init(CURL*& conn, char* url)
{
	CURLcode code;

	conn = curl_easy_init();

	if (conn == NULL)
	{
		fprintf(stderr, "Failed to create CURL connection\n");
		exit(EXIT_FAILURE);
	}

	code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set error buffer [%d]\n", code);
		return false;
	}

	//additional options
	code = curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, 0);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to disable verifying the peer in ssl connection [%d]\n", code);
		return false;
	}
	code = curl_easy_setopt(conn, CURLOPT_USERAGENT, "ParadoxGameConverters");
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set useragent [%d]\n", code);
		return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_URL, url);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
		return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
		return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set writer [%s]\n", errorBuffer);
		return false;
	}

	code = curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer);
	if (code != CURLE_OK)
	{
		fprintf(stderr, "Failed to set write data [%s]\n", errorBuffer);
		return false;
	}
	
	return true;
}

bool isUpdateAvailable(const std::string& versionFilePath, const std::string& tagsUrl)
{
	CURL* conn = nullptr;
	CURLcode code;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	// Initialize CURL connection
	if (!init(conn, const_cast<char*>(tagsUrl.c_str())))
		return false;

	// Retrieve content for the URL
	code = curl_easy_perform(conn);
	curl_easy_cleanup(conn);

	if (code == CURLE_OK)
	{
		const VersionParser versionParser(versionFilePath);
		if (isVersionNewer(versionParser.getVersion(), getAllNumberedVersions(buffer)))
			return true;
	}
	return false;
}