/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Lequel? language identification based on trigrams
/// @author Marc S. Ressl
/// @copyright Copyright (c) 2022-2023
/// @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a


/* *****************************************************************
    * HEADER CONFIGURATION *
   ***************************************************************** */

#ifndef LEQUEL_H
#define LEQUEL_H

//* NECESSARY LIBRARIES & HEADERS

#include <list>
#include <map>
#include <string>

#include "Text.h"


//* DATA TYPES & STRUCTURES

// TrigramProfile: map of trigram -> frequency
typedef std::map<std::string, float> TrigramProfile;

// TrigramList: list of trigrams
typedef std::list<std::string> TrigramList;

struct LanguageProfile
{
    std::string languageCode;
    TrigramProfile trigramProfile;
};

typedef std::list<LanguageProfile> LanguageProfiles;


//* PUBLIC FUNCTION PROTOTYPES

TrigramProfile buildTrigramProfile(const Text &text, size_t maxCharsToProcess);
void normalizeTrigramProfile(TrigramProfile &trigramProfile);
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile);
std::string identifyLanguage(const Text &text, LanguageProfiles &languages);


#endif // LEQUEL_H
