/* *****************************************************************
    * FILE INFORMATION *
   ***************************************************************** */
   
/// @brief Lequel? language identification based on trigrams
/// @author Marc S. Ressl, Ian A. Dib, Ana Destefano Testa
/// @copyright Copyright (c) 2022-2023
/// @cite https://towardsdatascience.com/understanding-cosine-similarity-and-its-application-fd42f585296a


/* *****************************************************************
    * FILE CONFIGURATION *
   ***************************************************************** */

//* NECESSARY LIBRARIES & HEADERS

#include <cmath>
#include <codecvt>
#include <locale>
#include <iostream>
#include <cwctype> // Implements the towlower() function
using namespace std;

#include "Lequel.h"


//* CONSTANTS

// Limit of characters to process
#define MAX_CHARS 10000


/* *****************************************************************
    * LOGIC MODULES *
   ***************************************************************** */

//* TRIGRAM PROFILE MANAGEMENT

/// @brief Builds a trigram profile from a given text
/// @param text Vector of lines (Text)
/// @return The trigram profile
TrigramProfile buildTrigramProfile(const Text &text, size_t maxCharsToProcess)
{
    // Variables to limit text processing
    size_t totalProcessedChars = 0;
    bool limitReached = false;
    
    wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    // profile is the map container where trigrams (keys) and their frequencies (values) are stored
    TrigramProfile profile; 

    for (auto line : text)
    {
        // Check if the character limit has been reached
        if (limitReached)
            break;
        
        if ((line.length() > 0) && (line[line.length() - 1] == '\r'))
            line = line.substr(0, line.length() - 1);

        // Converts UTF-8 string to wstring
        wstring unicodeString = converter.from_bytes(line); 

        // Normalize text to lowercase
        wstring unicodeStringLower;
        for (wchar_t c : unicodeString)
        {
            unicodeStringLower += towlower(c);
        }
        unicodeString = unicodeStringLower;

        // If the line has less than 3 characters, the program ignores it
        if (unicodeString.length() < 3)
            continue;

        for (unsigned int i = 0; i <= unicodeString.length() - 3; i++)
        {
            wstring unicodeTrigram = unicodeString.substr(i, 3);

            // Convert wstring to UTF-8 string
            string trigram = converter.to_bytes(unicodeTrigram);

            // Counts trigram appearances
            profile[trigram]++; 
        }

        // Update the character counter, and the character limit flag if necessary
        totalProcessedChars += unicodeString.length();
        if (totalProcessedChars >= maxCharsToProcess)
            limitReached = true;
    }

    return profile;
}


/// @brief Normalizes a trigram profile
/// @param trigramProfile The trigram profile
void normalizeTrigramProfile(TrigramProfile &trigramProfile)
{
    float sumSquares = 0.0;
    
    // Calculate the sum of the squares of the frequencies
    for (auto &element : trigramProfile)
    {
        sumSquares += element.second * element.second;
    }
    
    // Calculate the square root of the sum of the square of the frequencies
    float normalizeFactor = sqrt(sumSquares);

    // Normalize each frequency, avoid dividing by 0
    if (normalizeFactor > 0)
    {
        for (auto &element : trigramProfile)
        {
            // Note: element.second is the value associated with each key
            element.second /= normalizeFactor; 
        }
    }

    return;
}


//* COSINE SIMILARITY

/// @brief Calculates the cosine similarity between two trigram profiles
/// @param textProfile The text trigram profile (normalized)
/// @param languageProfile The language trigram profile (normalized)
/// @return The cosine similarity score (0.0-1.0, where 1.0 is identical)
float getCosineSimilarity(TrigramProfile &textProfile, TrigramProfile &languageProfile)
{
    // Cosine similarity for normalized vectors is the dot product of the vectors
    float similarity = 0.0;
    
    // For each trigram in the language profile, check if it exists in the text
    for (auto& languageElement : languageProfile)
    {
        // Get the trigram we're checking
        string trigram = languageElement.first;
        
        // Look for this trigram in the text profile
        auto textElementIterator = textProfile.find(trigram);
        
        // If the trigram exists in both profiles, multiply their frequencies
        if (textElementIterator != textProfile.end())  
        {
            float languageFrequency = languageElement.second;
            float textFrequency = textElementIterator->second;
            
            // Add the product to our running similarity score
            similarity += languageFrequency * textFrequency;
        }
    }

    return similarity;
}


//* INTEGRATION MODULE

/// @brief Identifies the language of a text
/// @param text A Text (vector of lines)
/// @param languages A list of Language objects
/// @return The language code of the most likely language (string)
string identifyLanguage(const Text &text, LanguageProfiles &languages)
{
    // Build the trigram profile for the input text, analyzing up to a max amount of characters
    TrigramProfile textProfile = buildTrigramProfile(text, MAX_CHARS);
    
    // Normalize the text profile frequencies
    normalizeTrigramProfile(textProfile);
    
    float maxSimilarity = 0.0;
    string bestLanguageCode = "";

    // Find the language that gives the highest cosine similarity with the text
    for (auto& language : languages)
    {
        // Calculate cosine similarity between text profile and current language profile
        float similarity = getCosineSimilarity(textProfile, language.trigramProfile);
        
        // If this language has higher similarity than previous best, update the best
        if (similarity > maxSimilarity)
        {
            maxSimilarity = similarity;
            bestLanguageCode = language.languageCode;
        }
    }

    return bestLanguageCode;
}
