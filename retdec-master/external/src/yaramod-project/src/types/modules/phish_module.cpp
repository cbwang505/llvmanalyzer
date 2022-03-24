/**
 * @file src/types/modules/phish_module.cpp
 * @brief Implementation of PhishModule.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include "yaramod/types/expression.h"
#include "yaramod/types/modules/phish_module.h"
#include "yaramod/types/symbol.h"

namespace yaramod {

/**
 * Constructor.
 */
PhishModule::PhishModule() : Module("phish", ImportFeatures::AvastOnly)
{
}

/**
 * Initializes module structure.
 *
 * @return @c true if success, otherwise @c false.
 */
bool PhishModule::initialize(ImportFeatures)
{
	using Type = Expression::Type;

	auto phishStruct = std::make_shared<StructureSymbol>("phish");
	phishStruct->addAttribute(std::make_shared<ValueSymbol>("source_url", Type::String));

	auto fileContentsStruct = std::make_shared<StructureSymbol>("file_contents");
	fileContentsStruct->addAttribute(std::make_shared<ValueSymbol>("favicon", Type::String));
	fileContentsStruct->addAttribute(std::make_shared<ValueSymbol>("title", Type::String));

	auto aStruct = std::make_shared<StructureSymbol>("a");
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("href", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("href", Type::Int, Type::Regexp));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("hrefs_hash", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::Regexp));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("ids_hash", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("style", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("style", Type::Int, Type::Regexp));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("styles_hash", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("text", Type::Int, Type::String));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("text", Type::Int, Type::Regexp));
	aStruct->addAttribute(std::make_shared<FunctionSymbol>("texts_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(aStruct);

	auto buttonStruct = std::make_shared<StructureSymbol>("button");
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::String));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::Regexp));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("ids_hash", Type::Int, Type::String));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("onclick", Type::Int, Type::String));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("onclick", Type::Int, Type::Regexp));
	buttonStruct->addAttribute(std::make_shared<FunctionSymbol>("onclicks_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(buttonStruct);

	auto commentStruct = std::make_shared<StructureSymbol>("comment");
	commentStruct->addAttribute(std::make_shared<FunctionSymbol>("text_partial_hash", Type::Int, Type::String));
	commentStruct->addAttribute(std::make_shared<FunctionSymbol>("texts_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(commentStruct);

	auto divStruct = std::make_shared<StructureSymbol>("div");
	divStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	divStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	divStruct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	divStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::String));
	divStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::Regexp));
	divStruct->addAttribute(std::make_shared<FunctionSymbol>("ids_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(divStruct);

	auto formStruct = std::make_shared<StructureSymbol>("form");
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("action", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("action", Type::Int, Type::Regexp));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("actions_hash", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("method", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("method", Type::Int, Type::Regexp));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("methods_hash", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::String));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::Regexp));
	formStruct->addAttribute(std::make_shared<FunctionSymbol>("names_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(formStruct);

	auto h1Struct = std::make_shared<StructureSymbol>("h1");
	h1Struct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	h1Struct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	h1Struct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	h1Struct->addAttribute(std::make_shared<FunctionSymbol>("text", Type::Int, Type::String));
	h1Struct->addAttribute(std::make_shared<FunctionSymbol>("text", Type::Int, Type::Regexp));
	h1Struct->addAttribute(std::make_shared<FunctionSymbol>("texts_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(h1Struct);

	auto h2Struct = std::make_shared<StructureSymbol>("h2");
	h2Struct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	h2Struct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	h2Struct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	h2Struct->addAttribute(std::make_shared<FunctionSymbol>("text", Type::Int, Type::String));
	h2Struct->addAttribute(std::make_shared<FunctionSymbol>("text", Type::Int, Type::Regexp));
	h2Struct->addAttribute(std::make_shared<FunctionSymbol>("texts_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(h2Struct);

	auto iframeStruct = std::make_shared<StructureSymbol>("iframe");
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("width", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("width", Type::Int, Type::Regexp));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("widths_hash", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("height", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("height", Type::Int, Type::Regexp));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("heights_hash", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::Regexp));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("srcs_hash", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("style", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("style", Type::Int, Type::Regexp));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("styles_hash", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::String));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::Regexp));
	iframeStruct->addAttribute(std::make_shared<FunctionSymbol>("names_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(iframeStruct);

	auto imgStruct = std::make_shared<StructureSymbol>("img");
	imgStruct->addAttribute(std::make_shared<FunctionSymbol>("alt", Type::Int, Type::String));
	imgStruct->addAttribute(std::make_shared<FunctionSymbol>("alt", Type::Int, Type::Regexp));
	imgStruct->addAttribute(std::make_shared<FunctionSymbol>("alts_hash", Type::Int, Type::String));
	imgStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::String));
	imgStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::Regexp));
	imgStruct->addAttribute(std::make_shared<FunctionSymbol>("srcs_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(imgStruct);

	auto inputStruct = std::make_shared<StructureSymbol>("input");
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("id", Type::Int, Type::Regexp));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("ids_hash", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::Regexp));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("names_hash", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("placeholder", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("placeholder", Type::Int, Type::Regexp));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("placeholders_hash", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::Regexp));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("srcs_hash", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::Regexp));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("types_hash", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("value", Type::Int, Type::String));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("value", Type::Int, Type::Regexp));
	inputStruct->addAttribute(std::make_shared<FunctionSymbol>("values_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(inputStruct);

	auto linkStruct = std::make_shared<StructureSymbol>("link");
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("href", Type::Int, Type::String));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("href", Type::Int, Type::Regexp));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("hrefs_hash", Type::Int, Type::String));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("rel", Type::Int, Type::String));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("rel", Type::Int, Type::Regexp));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("rels_hash", Type::Int, Type::String));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::String));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::Regexp));
	linkStruct->addAttribute(std::make_shared<FunctionSymbol>("types_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(linkStruct);

	auto metaTagStruct = std::make_shared<StructureSymbol>("meta_tag");
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("content", Type::Int, Type::String));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("content", Type::Int, Type::Regexp));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("contents_hash", Type::Int, Type::String));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::String));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("name", Type::Int, Type::Regexp));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("names_hash", Type::Int, Type::String));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("property", Type::Int, Type::String));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("property", Type::Int, Type::Regexp));
	metaTagStruct->addAttribute(std::make_shared<FunctionSymbol>("properties_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(metaTagStruct);

	auto scriptStruct = std::make_shared<StructureSymbol>("script");
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::String));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("src", Type::Int, Type::Regexp));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("srcs_hash", Type::Int, Type::String));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("text_partial_hash", Type::Int, Type::String));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("texts_hash", Type::Int, Type::String));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("text_lengths_hash", Type::Int, Type::String));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::String));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::Regexp));
	scriptStruct->addAttribute(std::make_shared<FunctionSymbol>("types_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(scriptStruct);

	auto spanStruct = std::make_shared<StructureSymbol>("span");
	spanStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::String));
	spanStruct->addAttribute(std::make_shared<FunctionSymbol>("class", Type::Int, Type::Regexp));
	spanStruct->addAttribute(std::make_shared<FunctionSymbol>("classes_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(spanStruct);

	auto styleStruct = std::make_shared<StructureSymbol>("style");
	styleStruct->addAttribute(std::make_shared<FunctionSymbol>("text_partial_hash", Type::Int, Type::String));
	styleStruct->addAttribute(std::make_shared<FunctionSymbol>("texts_hash", Type::Int, Type::String));
	styleStruct->addAttribute(std::make_shared<FunctionSymbol>("text_lengths_hash", Type::Int, Type::String));
	styleStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::String));
	styleStruct->addAttribute(std::make_shared<FunctionSymbol>("type", Type::Int, Type::Regexp));
	styleStruct->addAttribute(std::make_shared<FunctionSymbol>("types_hash", Type::Int, Type::String));
	fileContentsStruct->addAttribute(styleStruct);
	phishStruct->addAttribute(fileContentsStruct);

	auto fileAnalysisStruct = std::make_shared<StructureSymbol>("file_analysis");
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("base64_img_count", Type::Int));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("company_name", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("content_type", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("dead_link_count", Type::Int));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("detected_by", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("favicon_path_type", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("iframe_zero_size_count", Type::Int));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("phish_group", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("script_total_text_length", Type::Int));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("similarity_crc32", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("similarity_hash", Type::String));
	fileAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("style_total_text_length", Type::Int));
	phishStruct->addAttribute(fileAnalysisStruct);

	auto urlContentsStruct = std::make_shared<StructureSymbol>("url_contents");
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("url", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("scheme", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("path", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("params", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("query", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("fragment", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("username", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("password", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("hostname", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("port", Type::Int));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("is_ipv4", Type::Int));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("domain_name", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("subdomains", Type::String));
	urlContentsStruct->addAttribute(std::make_shared<ValueSymbol>("tld", Type::String));
	phishStruct->addAttribute(urlContentsStruct);

	auto urlAnalysisStruct = std::make_shared<StructureSymbol>("url_analysis");
	urlAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("suspicious_source_tld", Type::Int));
	urlAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("not_valid_domain", Type::Int));
	urlAnalysisStruct->addAttribute(std::make_shared<ValueSymbol>("url_type", Type::String));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("domain_key_word", Type::Int, Type::String));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("domain_key_word", Type::Int, Type::Regexp));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("domain_key_words_hash", Type::Int, Type::String));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("path_key_word", Type::Int, Type::String));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("path_key_word", Type::Int, Type::Regexp));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("path_key_words_hash", Type::Int, Type::String));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("url_sequence", Type::Int, Type::String));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("url_sequence", Type::Int, Type::Regexp));
	urlAnalysisStruct->addAttribute(std::make_shared<FunctionSymbol>("url_sequences_hash", Type::Int, Type::String));
	phishStruct->addAttribute(urlAnalysisStruct);

	_structure = phishStruct;
	return true;
}

}
