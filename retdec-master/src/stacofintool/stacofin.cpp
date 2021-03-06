/**
 * @file src/stacofintool/stacofin.cpp
 * @brief Static code detection tool.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <iomanip>
#include <string>
#include <vector>

#include "retdec/utils/filesystem.h"
#include "retdec/utils/io/log.h"
#include "retdec/utils/version.h"
#include "retdec/stacofin/stacofin.h"
#include "retdec/loader/image_factory.h"

using namespace retdec::utils;
using namespace retdec::utils::io;
using namespace retdec::stacofin;
using namespace retdec::loader;

/**
 * Print usage.
 */
void printUsage()
{
	Log::info() << "\nStatic code detection tool.\n"
		<< "Usage: stacofin -b BINARY_FILE YARA_FILE [YARA_FILE ...]\n\n";
}

/**
 * Print error message and return non-zero value.
 *
 * @param errorMessage message to print
 * @return non-zero value
 */
int printError(
	const std::string &errorMessage)
{
	Log::error() << Log::Error << errorMessage << "\n";
	return 1;
}

/**
 * Convert reference pairs to string.
 *
 * @param references references
 * @return references as string
 */
std::string referencesToString(
	const References &references)
{
	std::string result;
	for (const auto &ref : references) {
		result += std::to_string(ref.offset) + " " + ref.name + " ";
	}

	return result;
}

/**
 * Print results for debug purposes.
 *
 * @param detections detected functions
 */
void printDetectionsDebug(
	const retdec::stacofin::DetectedFunctionsMultimap &detections)
{
	std::uint64_t lastAddress = 0;
	for (const auto& p : detections) {
		auto& detected = p.second;
		if (detected.getAddress() == lastAddress) {
			for (const auto &name : detected.names) {
				Log::info() << "or " << name << "\n";
			}
			continue;
		}
		lastAddress = detected.getAddress();
		std::ostringstream ss;
		ss << "0x" << std::setfill('0') << std::setw(8) << std::hex
			<< detected.getAddress() << " " << detected.names[0] << "\n";
		Log::info() << ss.str();
		for (std::size_t i = 1; i < detected.names.size(); ++i) {
			Log::info() << "or " << detected.names[i] << "\n";
		}
	}
}

/**
 * Print results.
 *
 * @param detections detected functions
 */
void printDetections(
	const retdec::stacofin::DetectedFunctionsMultimap &detections)
{
	std::uint64_t lastAddress = 0;
	for (const auto& p : detections) {
		auto& detected = p.second;
		if (detected.getAddress() == lastAddress) {
			for (const auto &name : detected.names) {
				Log::info() << "\t\t\t" << name << " "
					<< referencesToString(detected.references) << "\n";;
			}
			continue;
		}
		lastAddress = detected.getAddress();
		std::ostringstream ss;
		ss << "0x" << std::hex << detected.getAddress() << " \t"
			<< std::dec << detected.size << "\t" << detected.names[0] << " "
			<< referencesToString(detected.references) << "\n";
		Log::info() << ss.str();
		for (std::size_t i = 1; i < detected.names.size(); ++i) {
			Log::info() << "\t\t\t" << detected.names[i] << " "
				<< referencesToString(detected.references) << "\n";;
		}
	}
}

/**
 * Do actions according to command line arguments.
 *
 * @param args command line arguments
 */
int doActions(
	const std::vector<std::string> &args)
{
	bool debugOn = false;
	std::string binaryPath;
	std::vector<std::string> yaraPaths;

	for (std::size_t i = 0; i < args.size(); ++i) {
		if (args[i] == "-h" || args[i] == "--help") {
			printUsage();
			return 0;
		}
		else if (args[i] == "--version") {
			Log::info() << retdec::utils::version::getVersionStringLong()
					<< "\n";
			return 0;
		}
		else if (args[i] == "-d" || args[i] == "--debug") {
			debugOn = true;
		}
		else if (args[i] == "-b" && i + 1 < args.size()) {
			binaryPath = args[++i];
			if (!fs::is_regular_file(binaryPath)) {
				return printError("invalid binary file '" + binaryPath + "'");
			}
		}
		else {
			if (!fs::is_regular_file(args[i])) {
				return printError("invalid yara file '" + args[i] + "'");
			}
			yaraPaths.push_back(args[i]);
		}
	}

	// Load image.
	auto image = createImage(binaryPath);
	if (!image) {
		return printError("could not load binary file");
	}

	// Do search.
	Finder codeFinder;
	for (const auto &yaraPath : yaraPaths) {
		codeFinder.search(*image.get(), yaraPath);
	}

	// Print detections.
	if (debugOn) {
		printDetectionsDebug(codeFinder.getAllDetections());
	}
	else {
		printDetections(codeFinder.getAllDetections());
	}

	// Print total code coverage information.
	std::size_t totalCoverage = 0;
	auto coverage = codeFinder.getCoveredCode();
	for (auto it = coverage.begin(), e = coverage.end(); it != e; ++it) {
		totalCoverage += it->getSize();
	}
	std::ostringstream ss;
	ss << "\nTotal code coverage is " << totalCoverage << " bytes.\n";
	Log::info() <<ss.str();
	return 0;
}

int main(int argc, char *argv[])
{
	return doActions(std::vector<std::string>(argv + 1, argv + argc));
}
